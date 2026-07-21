/*
 * v4l2_info.c - 读取 V4L2 设备详细信息
 * 用法: ./v4l2_info [/dev/video0]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

static void print_caps(uint32_t caps)
{
    struct { uint32_t bit; const char *name; } tab[] = {
        { V4L2_CAP_VIDEO_CAPTURE,          "VIDEO_CAPTURE"          },
        { V4L2_CAP_VIDEO_CAPTURE_MPLANE,   "VIDEO_CAPTURE_MPLANE"  },
        { V4L2_CAP_VIDEO_OUTPUT,           "VIDEO_OUTPUT"           },
        { V4L2_CAP_VIDEO_OUTPUT_MPLANE,    "VIDEO_OUTPUT_MPLANE"   },
        { V4L2_CAP_VIDEO_OVERLAY,          "VIDEO_OVERLAY"          },
        { V4L2_CAP_READWRITE,              "READWRITE"              },
        { V4L2_CAP_ASYNCIO,                "ASYNCIO"                },
        { V4L2_CAP_STREAMING,              "STREAMING"              },
        { V4L2_CAP_DEVICE_CAPS,            "DEVICE_CAPS"            },
        { 0, NULL }
    };

    for (int i = 0; tab[i].name; i++) {
        if (caps & tab[i].bit)
            printf("    %s\n", tab[i].name);
    }
}

static void print_fmtdesc(int fd, uint32_t type)
{
    struct v4l2_fmtdesc f = { .type = type };
    const char *type_str = (type == V4L2_BUF_TYPE_VIDEO_CAPTURE) ?
                           "CAPTURE" : "OUTPUT";

    printf("  %s formats:\n", type_str);

    while (ioctl(fd, VIDIOC_ENUM_FMT, &f) == 0) {
        char pix[5];
        pix[0] = (f.pixelformat >> 0)  & 0xff;
        pix[1] = (f.pixelformat >> 8)  & 0xff;
        pix[2] = (f.pixelformat >> 16) & 0xff;
        pix[3] = (f.pixelformat >> 24) & 0xff;
        pix[4] = '\0';

        printf("    %s  [%s]%s%s\n",
               pix, f.description,
               (f.flags & V4L2_FMT_FLAG_COMPRESSED)  ? " COMPRESSED" : "",
               (f.flags & V4L2_FMT_FLAG_EMULATED)    ? " EMULATED"   : "");

        /* 枚举该格式支持的帧尺寸 */
        struct v4l2_frmsizeenum fs = { .pixel_format = f.pixelformat };
        while (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &fs) == 0) {
            if (fs.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
                printf("      %ux%u\n",
                       fs.discrete.width, fs.discrete.height);

                /* 枚举该尺寸支持的帧率 */
                struct v4l2_frmivalenum fi = {
                    .pixel_format = f.pixelformat,
                    .width        = fs.discrete.width,
                    .height       = fs.discrete.height,
                };
                printf("        fps:");
                while (ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &fi) == 0) {
                    if (fi.type == V4L2_FRMIVAL_TYPE_DISCRETE) {
                        printf(" %.0f", (float)fi.discrete.denominator
                                        / fi.discrete.numerator);
                    } else {
                        printf(" %.4f-%.4f",
                               (float)fi.stepwise.min.denominator
                               / fi.stepwise.min.numerator,
                               (float)fi.stepwise.max.denominator
                               / fi.stepwise.max.numerator);
                        break;
                    }
                    fi.index++;
                }
                printf("\n");
            } else if (fs.type == V4L2_FRMSIZE_TYPE_STEPWISE) {
                printf("      %ux%u - %ux%u (step %ux%u)\n",
                       fs.stepwise.min_width,  fs.stepwise.min_height,
                       fs.stepwise.max_width,  fs.stepwise.max_height,
                       fs.stepwise.step_width, fs.stepwise.step_height);
            }
            fs.index++;
        }
        f.index++;
    }
}

static const char *ctrl_type_str(unsigned type)
{
    switch (type) {
    case V4L2_CTRL_TYPE_INTEGER:      return "INTEGER";
    case V4L2_CTRL_TYPE_BOOLEAN:      return "BOOLEAN";
    case V4L2_CTRL_TYPE_MENU:         return "MENU";
    case V4L2_CTRL_TYPE_BUTTON:       return "BUTTON";
    case V4L2_CTRL_TYPE_INTEGER64:    return "INTEGER64";
    case V4L2_CTRL_TYPE_CTRL_CLASS:   return "CTRL_CLASS";
    case V4L2_CTRL_TYPE_STRING:       return "STRING";
    case V4L2_CTRL_TYPE_BITMASK:      return "BITMASK";
    case V4L2_CTRL_TYPE_INTEGER_MENU: return "INTEGER_MENU";
    case V4L2_CTRL_TYPE_U8:           return "U8";
    case V4L2_CTRL_TYPE_U16:          return "U16";
    case V4L2_CTRL_TYPE_U32:          return "U32";
    case V4L2_CTRL_TYPE_AREA:         return "AREA";
    default:                          return "UNKNOWN";
    }
}

static void print_ctrl(int fd)
{
    struct v4l2_queryctrl q = { 0 };

    printf("  Controls:\n");
    q.id = V4L2_CTRL_FLAG_NEXT_CTRL;

    while (ioctl(fd, VIDIOC_QUERYCTRL, &q) == 0) {
        if (q.flags & V4L2_CTRL_FLAG_DISABLED) {
            q.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
            continue;
        }

        printf("    %-32.32s [%s] %d-%d step=%d def=%d",
               (const char *)q.name,
               ctrl_type_str(q.type),
               q.minimum, q.maximum, q.step, q.default_value);

        if (q.flags & V4L2_CTRL_FLAG_READ_ONLY)
            printf(" READ_ONLY");
        printf("\n");

        /* 菜单项 */
        if (q.type == V4L2_CTRL_TYPE_MENU) {
            struct v4l2_querymenu m = { .id = q.id };
            for (m.index = q.minimum;
                 (int)m.index <= q.maximum &&
                 ioctl(fd, VIDIOC_QUERYMENU, &m) == 0;
                 m.index++) {
                printf("      %d: %.32s\n", m.index, (const char *)m.name);
            }
        }
        q.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
    }
}

int main(int argc, char *argv[])
{
    const char *dev = argc > 1 ? argv[1] : "/dev/video0";
    int fd = open(dev, O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct v4l2_capability cap;
    struct v4l2_format      fmt;
    struct v4l2_streamparm  parm;

    /* --- 驱动信息 --- */
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        perror("VIDIOC_QUERYCAP");
        close(fd);
        return 1;
    }

    printf("=== %s ===\n\n", dev);
    printf("Driver:    %s\n", cap.driver);
    printf("Card:      %s\n", cap.card);
    printf("Bus:       %s\n", cap.bus_info);
    printf("Version:   %u.%u.%u\n",
           (cap.version >> 16) & 0xff,
           (cap.version >>  8) & 0xff,
           (cap.version >>  0) & 0xff);
    printf("Capabilities:\n");
    print_caps(cap.device_caps ? cap.device_caps : cap.capabilities);
    printf("\n");

    /* --- 当前格式 --- */
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_G_FMT, &fmt) == 0) {
        char pix[5];
        pix[0] = (fmt.fmt.pix.pixelformat >> 0)  & 0xff;
        pix[1] = (fmt.fmt.pix.pixelformat >> 8)  & 0xff;
        pix[2] = (fmt.fmt.pix.pixelformat >> 16) & 0xff;
        pix[3] = (fmt.fmt.pix.pixelformat >> 24) & 0xff;
        pix[4] = '\0';

        printf("Current format:\n");
        printf("  %ux%u  %s  stride=%u  size=%u\n\n",
               fmt.fmt.pix.width,
               fmt.fmt.pix.height,
               pix,
               fmt.fmt.pix.bytesperline,
               fmt.fmt.pix.sizeimage);
    }

    /* --- 帧率 --- */
    memset(&parm, 0, sizeof(parm));
    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_G_PARM, &parm) == 0) {
        struct v4l2_fract *fps = &parm.parm.capture.timeperframe;
        if (fps->numerator && fps->denominator)
            printf("FPS:       %.2f\n\n",
                   (float)fps->denominator / fps->numerator);
    }

    /* --- 输入 --- */
    {
        struct v4l2_input inp;
        int idx = 0;
        printf("Inputs:\n");
        while (ioctl(fd, VIDIOC_ENUMINPUT, &inp) == 0) {
            inp.index = idx++;
            printf("  %d: %s\n", inp.index, inp.name);
        }
        printf("\n");
    }

    /* --- 支持格式（含分辨率/帧率） --- */
    if (cap.device_caps & V4L2_CAP_VIDEO_CAPTURE)
        print_fmtdesc(fd, V4L2_BUF_TYPE_VIDEO_CAPTURE);
    if (cap.device_caps & V4L2_CAP_VIDEO_OUTPUT)
        print_fmtdesc(fd, V4L2_BUF_TYPE_VIDEO_OUTPUT);

    printf("\n");

    /* --- 控制项 --- */
    print_ctrl(fd);

    close(fd);
    return 0;
}
