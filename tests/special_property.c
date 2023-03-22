/**
 * Copyright (c) 2022 IotaHydrae
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <stdio.h>
#include <sys/time.h>

#define pr_debug(fmt, arg...) printf("%s, %s:%d: " fmt, __FILE__, __func__, __LINE__, ##arg)
#define MAX_TRY_TIMES 10000000

struct foo {
    char a;
    unsigned b;
} __attribute__((aligned(4)));

struct foo2 {
    char a;
    unsigned b;
} __attribute__((packed));

struct foo3 {
    char a;
    unsigned b;
} __attribute__((aligned(4)));

int main(int argc, char **argv)
{
    struct timeval tv_start;
    struct timeval tv_end;

    struct foo foo = {
        .a = 'a',
        .b = 0,
    };
    
    gettimeofday(&tv_start, NULL);
    for(int i=0;i<MAX_TRY_TIMES;i++)
        foo.b++;
    gettimeofday(&tv_end, NULL);
    pr_debug("time in ms: %ld\n", tv_end.tv_usec - tv_start.tv_usec);

    struct foo2 foo2 = {
        .a = 'a',
        .b = 0,
    };

    gettimeofday(&tv_start, NULL);
    for(int i=0;i<MAX_TRY_TIMES;i++)
        foo2.b++;
    gettimeofday(&tv_end, NULL);

    pr_debug("time in ms: %ld\n", tv_end.tv_usec - tv_start.tv_usec);

    struct foo3 foo3 = {
        .a = 'a',
        .b = 0,
    };

    gettimeofday(&tv_start, NULL);
    for(int i=0;i<MAX_TRY_TIMES;i++)
        foo3.b++;
    gettimeofday(&tv_end, NULL);
    pr_debug("time in ms: %ld\n", tv_end.tv_usec - tv_start.tv_usec);
    
    return 0;
}