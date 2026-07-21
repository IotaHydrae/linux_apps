/*
 * v4l2_ctrl.c - 查询或设置 V4L2 设备控制参数
 * 用法:
 *   ./v4l2_ctrl /dev/video0              # 列出所有控制项及其当前值
 *   ./v4l2_ctrl /dev/video0 Brightness    # 查询单个控制项
 *   ./v4l2_ctrl /dev/video0 Brightness 150 # 设置 Brightness = 150
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

static const char *ctrl_type_name(unsigned type)
{
    switch (type) {
    case V4L2_CTRL_TYPE_INTEGER:      return "int";
    case V4L2_CTRL_TYPE_BOOLEAN:      return "bool";
    case V4L2_CTRL_TYPE_MENU:         return "menu";
    case V4L2_CTRL_TYPE_BUTTON:       return "button";
    case V4L2_CTRL_TYPE_INTEGER64:    return "int64";
    case V4L2_CTRL_TYPE_STRING:       return "str";
    case V4L2_CTRL_TYPE_BITMASK:      return "bitmask";
    case V4L2_CTRL_TYPE_INTEGER_MENU: return "int_menu";
    case V4L2_CTRL_TYPE_U8:           return "u8";
    case V4L2_CTRL_TYPE_U16:          return "u16";
    case V4L2_CTRL_TYPE_U32:          return "u32";
    default:                          return "?";
    }
}

/* 根据名字查找控制项 ID */
static int find_ctrl(int fd, const char *name, struct v4l2_queryctrl *out)
{
    struct v4l2_queryctrl q = { .id = V4L2_CTRL_FLAG_NEXT_CTRL };

    while (ioctl(fd, VIDIOC_QUERYCTRL, &q) == 0) {
        if (!(q.flags & V4L2_CTRL_FLAG_DISABLED) &&
            strcasecmp((const char *)q.name, name) == 0) {
            *out = q;
            return 0;
        }
        q.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
    }
    return -1;
}

/* 获取控制项的当前值 */
static int get_ctrl_val(int fd, unsigned id, int type, int64_t *val)
{
    struct v4l2_control c  = { .id = id };
    struct v4l2_ext_control ec = { .id = id };

    switch (type) {
    case V4L2_CTRL_TYPE_INTEGER64:
        if (ioctl(fd, VIDIOC_G_EXT_CTRLS,
                  &(struct v4l2_ext_controls){ .count = 1, .controls = &ec }) == 0) {
            *val = ec.value64;
            return 0;
        }
        return -1;
    case V4L2_CTRL_TYPE_STRING: {
        char buf[256] = { 0 };
        ec.size = sizeof(buf);
        ec.string = buf;
        if (ioctl(fd, VIDIOC_G_EXT_CTRLS,
                  &(struct v4l2_ext_controls){ .count = 1, .controls = &ec }) == 0) {
            printf("  value: \"%s\"\n", buf);
            return 0;
        }
        return -1;
    }
    default:
        if (ioctl(fd, VIDIOC_G_CTRL, &c) == 0) {
            *val = c.value;
            return 0;
        }
        return -1;
    }
}

/* 设置控制项的值 */
static int set_ctrl_val(int fd, unsigned id, int type, int64_t val)
{
    struct v4l2_control c  = { .id = id, .value = (int32_t)val };
    struct v4l2_ext_control ec = { .id = id, .value64 = val };

    switch (type) {
    case V4L2_CTRL_TYPE_INTEGER64:
        if (ioctl(fd, VIDIOC_S_EXT_CTRLS,
                  &(struct v4l2_ext_controls){ .count = 1, .controls = &ec }) == 0)
            return 0;
        return -1;
    case V4L2_CTRL_TYPE_BUTTON:
        c.value = 1; /* 按钮: 写入非零值触发 */
        if (ioctl(fd, VIDIOC_S_CTRL, &c) == 0)
            return 0;
        return -1;
    default:
        if (ioctl(fd, VIDIOC_S_CTRL, &c) == 0)
            return 0;
        return -1;
    }
}

/* 打印菜单项列表 */
static void print_menu_items(int fd, const struct v4l2_queryctrl *q)
{
    struct v4l2_querymenu m = { .id = q->id };
    printf(" [");
    for (m.index = q->minimum;
         (int)m.index <= q->maximum &&
         ioctl(fd, VIDIOC_QUERYMENU, &m) == 0;
         m.index++) {
        printf("%s%d:%.32s",
               m.index == q->minimum ? "" : "  ",
               m.index, (const char *)m.name);
    }
    printf("]\n");
}

/* 列出所有控制项及当前值 */
static void list_all(int fd)
{
    struct v4l2_queryctrl q = { .id = V4L2_CTRL_FLAG_NEXT_CTRL };
    int found = 0;

    while (ioctl(fd, VIDIOC_QUERYCTRL, &q) == 0) {
        if (q.flags & V4L2_CTRL_FLAG_DISABLED) {
            q.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
            continue;
        }

        /* 跳过控件类节点(只作为分组标签) */
        if (q.type == V4L2_CTRL_TYPE_CTRL_CLASS) {
            printf("\n[%s]\n", q.name);
            q.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
            continue;
        }

        printf("  %-32s", q.name);

        if (q.flags & V4L2_CTRL_FLAG_READ_ONLY) {
            printf("  [RO %s]", ctrl_type_name(q.type));
        } else {
            printf("  [%s]", ctrl_type_name(q.type));
        }

        /* 获取当前值 */
        int64_t val = 0;
        if (get_ctrl_val(fd, q.id, q.type, &val) == 0) {
            if (q.type == V4L2_CTRL_TYPE_BOOLEAN)
                printf("  = %s (range %d-%d)",
                       val ? "true" : "false",
                       q.minimum, q.maximum);
            else if (q.type == V4L2_CTRL_TYPE_STRING)
                ; /* STRING 的值已由 get_ctrl_val 内部打印 */
            else
                printf("  = %lld (range %d-%d  step=%d  def=%d)",
                       (long long)val,
                       q.minimum, q.maximum,
                       q.step, q.default_value);
        } else {
            printf("  (无法读取)");
        }

        if (q.flags & V4L2_CTRL_FLAG_WRITE_ONLY)
            printf(" WRITE_ONLY");

        printf("\n");

        if (q.type == V4L2_CTRL_TYPE_MENU)
            print_menu_items(fd, &q);

        q.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
        found++;
    }

    if (!found)
        printf("  (无可用控制项)\n");
}

static void print_usage(const char *prog)
{
    printf("用法:\n"
           "  %s <device>                  列出所有控制项及当前值\n"
           "  %s <device> <ctrl>           查询指定控制项\n"
           "  %s <device> <ctrl> <value>   设置控制项的值\n",
           prog, prog, prog);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    const char *dev = argv[1];
    int fd = open(dev, O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    /* 不带控制项名称: 列出全部 */
    if (argc == 2) {
        list_all(fd);
        close(fd);
        return 0;
    }

    /* 查找指定的控制项 */
    const char *name = argv[2];
    struct v4l2_queryctrl q;
    if (find_ctrl(fd, name, &q) != 0) {
        fprintf(stderr, "未找到控制项: %s\n", name);
        close(fd);
        return 1;
    }

    /* 查询 */
    if (argc == 3) {
        printf("%s [%s]  ", q.name, ctrl_type_name(q.type));

        if (q.flags & V4L2_CTRL_FLAG_WRITE_ONLY) {
            printf("WRITE_ONLY (无法读取)\n");
        } else {
            int64_t val = 0;
            if (get_ctrl_val(fd, q.id, q.type, &val) == 0) {
                if (q.type == V4L2_CTRL_TYPE_BOOLEAN)
                    printf("= %s\n", val ? "true" : "false");
                else if (q.type == V4L2_CTRL_TYPE_STRING)
                    ; /* 已打印 */
                else {
                    printf("= %lld\n", (long long)val);
                    printf("  range: %d - %d  step: %d  default: %d\n",
                           q.minimum, q.maximum, q.step, q.default_value);
                }
            } else {
                printf("(读取失败)\n");
            }
        }

        if (q.type == V4L2_CTRL_TYPE_MENU)
            print_menu_items(fd, &q);

        close(fd);
        return 0;
    }

    /* 设置 */
    if (argc == 4) {
        if (q.flags & V4L2_CTRL_FLAG_READ_ONLY) {
            fprintf(stderr, "%s 是只读的\n", q.name);
            close(fd);
            return 1;
        }

        int64_t val;
        if (q.type == V4L2_CTRL_TYPE_BOOLEAN) {
            if (strcasecmp(argv[3], "true")  == 0 ||
                strcasecmp(argv[3], "on")    == 0 ||
                strcasecmp(argv[3], "1")     == 0 ||
                strcasecmp(argv[3], "yes")   == 0)
                val = 1;
            else if (strcasecmp(argv[3], "false") == 0 ||
                     strcasecmp(argv[3], "off")   == 0 ||
                     strcasecmp(argv[3], "0")     == 0 ||
                     strcasecmp(argv[3], "no")    == 0)
                val = 0;
            else {
                fprintf(stderr, "布尔值请用 true/false 或 1/0\n");
                close(fd);
                return 1;
            }
        } else {
            val = atoll(argv[3]);
        }

        if (set_ctrl_val(fd, q.id, q.type, val) < 0) {
            perror("设置失败");
            close(fd);
            return 1;
        }

        printf("%s -> %lld\n", q.name, (long long)val);
        close(fd);
        return 0;
    }

    print_usage(argv[0]);
    close(fd);
    return 1;
}
