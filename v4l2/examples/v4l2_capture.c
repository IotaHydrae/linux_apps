/*
 * v4l2_capture.c - 简单的 V4L2 拍照程序
 * 用法: ./v4l2_capture [/dev/video0] [output.jpg]
 * 环境变量: WIDTH HEIGHT 可设置分辨率(默认640x480)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

#define WIDTH 640
#define HEIGHT 480

/* 优先尝试的像素格式列表 */
static const struct {
	uint32_t fourcc;
	const char *ext;
	const char *name;
} fmt_list[] = { { V4L2_PIX_FMT_MJPEG, ".jpg", "MJPEG" },
		 { V4L2_PIX_FMT_YUYV, ".raw", "YUYV" },
		 { V4L2_PIX_FMT_NV12, ".raw", "NV12" },
		 { 0, NULL, NULL } };

static int try_format(int fd, struct v4l2_format *fmt_out, int w, int h)
{
	struct v4l2_format fmt;

	for (int i = 0; fmt_list[i].fourcc; i++) {
		memset(&fmt, 0, sizeof(fmt));
		fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		fmt.fmt.pix.width = w;
		fmt.fmt.pix.height = h;
		fmt.fmt.pix.pixelformat = fmt_list[i].fourcc;
		fmt.fmt.pix.field = V4L2_FIELD_ANY;

		if (ioctl(fd, VIDIOC_S_FMT, &fmt) == 0) {
			printf("使用格式: %s %ux%u\n", fmt_list[i].name,
			       fmt.fmt.pix.width, fmt.fmt.pix.height);
			*fmt_out = fmt;
			return i;
		}
	}
	return -1;
}

int main(int argc, char *argv[])
{
	const char *dev = argc > 1 ? argv[1] : "/dev/video0";
	const char *out = argc > 2 ? argv[2] : NULL;
	int w = getenv("WIDTH") ? atoi(getenv("WIDTH")) : WIDTH;
	int h = getenv("HEIGHT") ? atoi(getenv("HEIGHT")) : HEIGHT;
	int fd, i, fmt_idx;
	struct v4l2_format fmt;
	struct v4l2_requestbuffers req;
	struct v4l2_buffer buf;
	void *buffer;
	size_t bufsize;
	char outfile[256];
	FILE *fp;

	/* 1. 打开设备 */
	fd = open(dev, O_RDWR);
	if (fd < 0) {
		perror("open");
		return 1;
	}

	/* 2. 检查设备能力 */
	struct v4l2_capability cap;
	if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
		perror("VIDIOC_QUERYCAP");
		close(fd);
		return 1;
	}
	if (!(cap.device_caps & V4L2_CAP_VIDEO_CAPTURE)) {
		fprintf(stderr,
			"%s 不是采集设备，请用 v4l2_info 查看可用的 /dev/video* 节点\n",
			dev);
		close(fd);
		return 1;
	}

	/* 3. 尝试设置格式 */
	fmt_idx = try_format(fd, &fmt, w, h);
	if (fmt_idx < 0) {
		fprintf(stderr,
			"不支持任何常用格式，请用 v4l2_info 查看 %s 支持的格式\n",
			dev);
		close(fd);
		return 1;
	}

	/* 生成输出文件名 */
	if (!out) {
		snprintf(outfile, sizeof(outfile), "capture%s",
			 fmt_list[fmt_idx].ext);
		out = outfile;
	}

	/* 3. 申请缓冲区 */
	memset(&req, 0, sizeof(req));
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	req.count = 1;

	if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
		perror("VIDIOC_REQBUFS");
		close(fd);
		return 1;
	}

	/* 4. 映射缓冲区 */
	memset(&buf, 0, sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = 0;

	if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
		perror("VIDIOC_QUERYBUF");
		close(fd);
		return 1;
	}

	bufsize = buf.length;
	buffer = mmap(NULL, bufsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
		      buf.m.offset);
	if (buffer == MAP_FAILED) {
		perror("mmap");
		close(fd);
		return 1;
	}

	/* 5. 缓冲区入队 */
	if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
		perror("VIDIOC_QBUF");
		munmap(buffer, bufsize);
		close(fd);
		return 1;
	}

	/* 6. 开启流 */
	i = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_STREAMON, &i) < 0) {
		perror("VIDIOC_STREAMON");
		munmap(buffer, bufsize);
		close(fd);
		return 1;
	}

	/* 7. 取一帧 */
	memset(&buf, 0, sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
		perror("VIDIOC_DQBUF");
		munmap(buffer, bufsize);
		close(fd);
		return 1;
	}

	/* 8. 保存到文件 */
	fp = fopen(out, "wb");
	if (!fp) {
		perror("fopen");
		munmap(buffer, bufsize);
		close(fd);
		return 1;
	}
	fwrite(buffer, buf.bytesused, 1, fp);
	fclose(fp);
	printf("保存 %u 字节到 %s\n", buf.bytesused, out);

	if (ioctl(fd, VIDIOC_STREAMOFF, &i) < 0) {
		perror("VIDIOC_STREAMOFF");
		munmap(buffer, bufsize);
		close(fd);
		return -1;
	}

	/* 9. 清理 */
	munmap(buffer, bufsize);
	close(fd);
	return 0;
}
