/**
 * Copyright (c) 2022 IotaHydrae
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <stdio.h>

#define MAX (10)

struct foo {
    void (*func)(int a);
};

void func(int a)
{
    printf("hello, %d\n", a);
}

struct foo  test = {
    .func = func
};

int main(int argc, char **argv)
{
    unsigned char data[MAX]  = { [0 ... MAX - 1] = 99  };

    for(int i=0;i<MAX;i++)
        printf("%d\n", data[i]);

    test.func(10);

    return 0;
}