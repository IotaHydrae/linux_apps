/*
 * v4l2_capture.c - 简单的 V4L2 拍照程序（支持连拍）
 * 用法:
 *   ./v4l2_capture [/dev/video0] [output] [count]
 * 默认文件名: frame.jpg (连拍: frame_000.jpg, frame_001.jpg ...)
 * 环境变量:
 *   WIDTH HEIGHT  分辨率(默认640x480)
 *   COUNT         拍摄张数(默认1, 命令行参数优先)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

#define WIDTH      640
#define HEIGHT     480
#define N_BUFS     2      /* 双缓冲 */

/* 优先尝试的像素格式列表 */
static const struct {
    uint32_t fourcc;
    const char *ext;
    const char *name;
} fmt_list[] = {
    { V4L2_PIX_FMT_MJPEG, ".jpg", "MJPEG" },
    { V4L2_PIX_FMT_YUYV,  ".raw", "YUYV"  },
    { V4L2_PIX_FMT_NV12,  ".raw", "NV12"  },
    { 0, NULL, NULL }
};

static int try_format(int fd, struct v4l2_format *fmt_out, int w, int h)
{
    struct v4l2_format fmt;

    for (int i = 0; fmt_list[i].fourcc; i++) {
        memset(&fmt, 0, sizeof(fmt));
        fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width       = w;
        fmt.fmt.pix.height      = h;
        fmt.fmt.pix.pixelformat = fmt_list[i].fourcc;
        fmt.fmt.pix.field       = V4L2_FIELD_ANY;

        if (ioctl(fd, VIDIOC_S_FMT, &fmt) == 0) {
            printf("使用格式: %s %ux%u\n",
                   fmt_list[i].name,
                   fmt.fmt.pix.width,
                   fmt.fmt.pix.height);
            *fmt_out = fmt;
            return i;
        }
    }
    return -1;
}

/* 分配并映射 N 个缓冲区 */
static int setup_buffers(int fd, int n, void **bufs, size_t *bufsize)
{
    struct v4l2_requestbuffers req = { 0 };
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    req.count  = n;

    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("VIDIOC_REQBUFS");
        return -1;
    }

    for (unsigned i = 0; i < req.count; i++) {
        struct v4l2_buffer buf = { 0 };
        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = i;

        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
            perror("VIDIOC_QUERYBUF");
            return -1;
        }

        bufs[i] = mmap(NULL, buf.length,
                       PROT_READ | PROT_WRITE,
                       MAP_SHARED, fd, buf.m.offset);
        if (bufs[i] == MAP_FAILED) {
            perror("mmap");
            return -1;
        }
        bufsize[i] = buf.length;
    }
    return req.count;
}

/* 把所有缓冲区入队 */
static int queue_all(int fd, int n)
{
    for (int i = 0; i < n; i++) {
        struct v4l2_buffer buf = { 0 };
        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = i;

        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("VIDIOC_QBUF");
            return -1;
        }
    }
    return 0;
}

/* 取一帧并保存, 返回填充的 buf index */
static int grab_frame(int fd, const char *filename, void **bufs, size_t *bufsize)
{
    struct v4l2_buffer buf = { 0 };
    buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
        perror("VIDIOC_DQBUF");
        return -1;
    }

    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("fopen");
        return -1;
    }
    fwrite(bufs[buf.index], buf.bytesused, 1, fp);
    fclose(fp);
    printf("  -> %s  (%u bytes)\n", filename, buf.bytesused);

    /* 归还缓冲区 */
    if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
        perror("VIDIOC_QBUF");
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    const char *dev = argc > 1 ? argv[1] : "/dev/video0";
    /* 智能参数解析: 纯数字 = count, 否则 = 文件名 */
    const char *out = NULL;
    int count = 1;
    int idx = 2;

    if (argc > idx && argv[idx][0]) {
        char *end;
        long n = strtol(argv[idx], &end, 10);
        if (*end == '\0' && n > 0) {
            /* 纯数字 → count */
            count = (int)n;
        } else {
            out = argv[idx];
            if (argc > ++idx)
                count = atoi(argv[idx]);
        }
    }
    if (count <= 1 && getenv("COUNT"))
        count = atoi(getenv("COUNT"));
    int w = getenv("WIDTH")  ? atoi(getenv("WIDTH"))  : WIDTH;
    int h = getenv("HEIGHT") ? atoi(getenv("HEIGHT")) : HEIGHT;

    if (count < 1) {
        fprintf(stderr, "count 必须 >= 1\n");
        return 1;
    }

    /* 1. 打开设备 */
    int fd = open(dev, O_RDWR);
    if (fd < 0) { perror("open"); return 1; }

    /* 2. 检查设备能力 */
    struct v4l2_capability cap;
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        perror("VIDIOC_QUERYCAP"); close(fd); return 1;
    }
    if (!(cap.device_caps & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr,
                "%s 不是采集设备，请用 v4l2_info 查看可用的 /dev/video* 节点\n",
                dev);
        close(fd); return 1;
    }

    /* 3. 设置格式 */
    struct v4l2_format fmt;
    int fmt_idx = try_format(fd, &fmt, w, h);
    if (fmt_idx < 0) {
        fprintf(stderr,
                "不支持任何常用格式，请用 v4l2_info 查看 %s 支持的格式\n", dev);
        close(fd); return 1;
    }

    /* 生成文件 basename 和扩展名 */
    const char *ext = fmt_list[fmt_idx].ext;
    char base[256];
    if (out) {
        /* 用户指定: 去掉扩展名作为 base */
        const char *dot = strrchr(out, '.');
        if (dot && dot != out) {
            size_t len = dot - out;
            if (len >= sizeof(base)) len = sizeof(base) - 1;
            memcpy(base, out, len);
            base[len] = '\0';
        } else {
            snprintf(base, sizeof(base), "%s", out);
        }
    } else {
        snprintf(base, sizeof(base), "frame");
    }

    /* 4. 分配缓冲区: 单张用 1 个, 连拍用双缓冲 */
    int nbuf = (count > 1) ? N_BUFS : 1;
    void  *bufs[N_BUFS];
    size_t bufsize[N_BUFS];

    int actual = setup_buffers(fd, nbuf, bufs, bufsize);
    if (actual < 0) { close(fd); return 1; }

    /* 5. 全部入队 */
    if (queue_all(fd, actual) < 0) {
        for (int i = 0; i < actual; i++) munmap(bufs[i], bufsize[i]);
        close(fd); return 1;
    }

    /* 6. 开启流 */
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        perror("VIDIOC_STREAMON");
        for (int i = 0; i < actual; i++) munmap(bufs[i], bufsize[i]);
        close(fd); return 1;
    }

    /* 7. 拍摄 count 张 */
    printf("拍摄 %d 张...\n", count);
    for (int n = 0; n < count; n++) {
        char fname[320];
        if (count == 1)
            snprintf(fname, sizeof(fname), "%s%s", base, ext);
        else
            snprintf(fname, sizeof(fname), "%s_%03d%s", base, n, ext);
        if (grab_frame(fd, fname, bufs, bufsize) < 0) {
            ioctl(fd, VIDIOC_STREAMOFF, &type);
            for (int i = 0; i < actual; i++) munmap(bufs[i], bufsize[i]);
            close(fd); return 1;
        }
    }

    /* 8. 停流 & 清理 */
    ioctl(fd, VIDIOC_STREAMOFF, &type);
    for (int i = 0; i < actual; i++)
        munmap(bufs[i], bufsize[i]);
    close(fd);

    printf("完成, 共 %d 张\n", count);
    return 0;
}
