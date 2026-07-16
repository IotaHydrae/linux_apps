#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <linux/fb.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


static int fd_fb;
static struct fb_var_screeninfo var;
static int screen_size;
static unsigned char *fb_base;
static unsigned char *hzkmem;

static unsigned int line_width,pixel_width;

void lcd_put_pixel(int x, int y, unsigned int color)
{
	unsigned char *pen_8 = fb_base + y*line_width + x*pixel_width;
	unsigned short *pen_16;
	unsigned int *pen_32;

	unsigned int red, green, blue;

	pen_16 = (unsigned short *)pen_8;
	pen_32 = (unsigned int *)pen_8;

	switch(var.bits_per_pixel)
	{
		case 8:
		{
			*pen_8 = color;
			break;
		}
		case 16:
		{
			red = (color >> 16) & 0xff;
			green = (color >> 8) & 0xff;
			blue = (color >> 0) & 0xff;
			color = ((red >> 3) << 11) | ((green >> 2) << 5) | ((blue >> 3) << 0);
			*pen_16 = color;
			break;
		}
		case 32:
		{
			*pen_32 = color;
			break;
		}
		default:
		{
			printf("can't surport %dbpp\n", var.bits_per_pixel);
		    break;
		}
	}
}

int main(int argc, char **argv)
{
	int fd_fb;
	int fd_hzk16;
	int i;
	unsigned int screen_size;

	fd_fb = open("/dev/fb0", O_RDWR);
	if(fd_fb < 0)
	{
		printf("can`t open /dev/fb0\n");
		return -1;
	}

	if(ioctl(fd_fb, FBIOGET_VSCREENINFO, &var))
	{
		printf("can`t get screen info\n");
		return -1;
	}
	line_width = var.xres * var.bits_per_pixel / 8;
	pixel_width = var.bits_per_pixel / 8;

	screen_size = var.xres * var.yres * var.bits_per_pixel / 8;

	printf("%d,%d,%d\n",var.xres,var.yres,var.bits_per_pixel);

	if(argc == 4){
		if(atoi(argv[1]) > 128 || atoi(argv[2]) > 64){
			printf("postion x or y is out of range! exiting...\n");
			return -1;
		}
	}

	fb_base = (unsigned char *)mmap(NULL, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb,0);

	if(fb_base == (unsigned char *)-1)
	{
		printf("can`t mmap\n");
		return -1;
	}

	memset(fb_base, 0, screen_size);
	printf("%s, cleaned screen\n.",__func__);

	/* render here */
	for(int x=0;x<240;x++){
		for(int y=0;y<280;y++){
			/*if(y==0) lcd_put_pixel(x,y,0xffffff);
			if(x==0) lcd_put_pixel(x,y,0xffffff);*/
			lcd_put_pixel(x,y,0xffffff);
		}
	}

	for(int x=0;x<120;x++){
		for(int y=0;y<140;y++){
			/*if(y==0) lcd_put_pixel(x,y,0xffffff);
			if(x==0) lcd_put_pixel(x,y,0xffffff);*/
			lcd_put_pixel(x,y,0xff);
		}
	}
	
	munmap(fb_base, screen_size);
	close(fd_fb);
	
	return 0;
}



