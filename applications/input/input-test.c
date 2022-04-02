#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <linux/input.h>

/* error handler */
#define handle_error(msg) do {perror(msg);exit(-1);} \
    while(0)

#define DEFAULT_INPUT_DEVICE "/dev/input/event0"

/* Input device */
typedef struct
{
    /*  */
    int fd;

    struct input_event indev_event;
} indev_struct;

static struct indev_struct indev;

void input_dev_sighandler(int signal)
{
	while(read(indev.fd, &indev.indev_event, sizeof(struct input_event)) > 0){
		printf("type : %d, code: %d, value: %d\n", indev.indev_event.type, indev.indev_event.code, indev.indev_event.value);
	}
}

int main(int argc, char **argv)
{

	indev.fd = open(DEFAULT_INPUT_DEVICE, O_RDWR);
	printf("Successfully open device in normal mode.\n");

    if(indev.fd < 0)    /* Returns -1 on error */
    {
        handle_error("can not open indev");
    }

    signal(SIGIO, input_dev_sighandler);
    fcntl(indev.fd, F_SETOWN, getpid());
    flags = fcntl(indev.fd, F_GETFL);
    fcntl(indev.fd, F_SETFL, flags | FASYNC | O_NONBLOCK);
    printf("Successfully run in async mode.\n");

	while(1);

	return 0;	
}
