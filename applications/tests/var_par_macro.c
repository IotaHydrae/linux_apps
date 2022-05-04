/**
 * Copyright (c) 2022 IotaHydrae
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <stdio.h>

#define pr_debug(fmt, arg...) printf("%s, %s:%d: " fmt, __FILE__, __func__, __LINE__, ##arg)

int main(int argc, char **argv)
{

    pr_debug("hello\n");
    return 0;
}