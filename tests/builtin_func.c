/**
 * Copyright (c) 2022 IotaHydrae
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <stdio.h>

#define pr_debug(fmt, arg...) printf("%s:%d: " fmt, __FILE__, __LINE__, ##arg)

int by_foo()
{
    pr_debug("%p\n", __builtin_return_address(1));
    return 0;
}

int foo()
{
    pr_debug("%p\n", __builtin_return_address(0));
    by_foo();
}

int main(int argc, char **argv)
{
    foo();

    return 0;
}