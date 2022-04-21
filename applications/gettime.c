#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>

int
main(int argc, char **argv)
{
    struct timeval tv;
    struct tm tm_res;

    while(1){
        gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &tm_res);
        printf("%02d:%02d:%02d\n",tm_res.tm_hour, tm_res.tm_min, tm_res.tm_sec);
        sleep(1);
    }

}
