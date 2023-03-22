#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <disp_manager.h>
#include <linux/fb.h>
#include <unistd.h>
#include <string.h>


static int fd_fb;
static int screen_size;
static unsigned char *fb_base;

static struct fb_var_screeninfo var;

int fb_device_init(void)
{
    printf("fb_device_init\n");
    fd_fb = open("/dev/fb0", O_RDWR);
    if(fd_fb < 0) {
        HANDLE_ERROR("can not open framebuffer!")
    }
    if(ioctl(fd_fb, FBIOGET_VSCREENINFO, &var) < 0) {
        HANDLE_ERROR("can not ioctl framebuffer!")
    }
    /* Alreay get the screen info */
    screen_size = var .xres * var.yres * var.bits_per_pixel / 8;
    fb_base = (unsigned char *)mmap(NULL, screen_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd_fb, 0);
    if(fb_base == (unsigned char *) -1) {
        HANDLE_ERROR("can not mmap framebuffer!")
    }
    memset(fb_base, 0x00, screen_size);
    return 0;
}

int fb_device_exit(void)
{
    munmap(fb_base, screen_size);
    close(fd_fb);
    return 0;
}

int fb_get_buffer(pdisp_buf pbuf)
{
    printf("fb_get_buffer\n");
    pbuf->iXres      = var.xres;
    pbuf->iYres      = var.yres;
    pbuf->iBpp       = var.bits_per_pixel;
    pbuf->cBuf_base  = fb_base;
    return 0;
}

int fb_flush_area(pdisp_buf buf, pdisp_area area)
{
    return 0;
}

static disp_opr opr = {
    .cName = "fb",
    .device_init    = fb_device_init,
    .device_exit    = fb_device_exit,
    .get_buffer     = fb_get_buffer,
    .flush_area     = fb_flush_area,
};

void FrameBufferInit(void)
{
    register_disp_drivers(&opr);
}


