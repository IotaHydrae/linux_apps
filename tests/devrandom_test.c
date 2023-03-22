#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void memory_comp(const unsigned char buf1[], const unsigned char buf2[], unsigned int size)
{
	printf("memory checking...\n");
	for(int i=0;i<size;i++)
	{
		if(buf1[i] != buf2[i])
		{
			printf("diff at %d: buf1-%d # buf2-%d", i, buf1[i], buf2[i]);
		}
	}
}

int main(int argc, char **argv)
{
	int fd;
	unsigned char buf[64], buf1[64];
	fd = open("/dev/urandom", O_RDONLY);
	while(1){
	read(fd, buf, sizeof(buf)/sizeof(buf[0]));

	for(int i=0;i<sizeof(buf)/sizeof(buf[0]);i++)
	{
		printf("%d,",buf[i]);
	}
	printf("\n");
	memcpy(buf, buf1, sizeof(buf)/sizeof(buf[0]));
	memory_comp(buf, buf1, sizeof(buf)/sizeof(buf[0]));
	sleep(1);
	}
	close(fd);

	return 0;
}
