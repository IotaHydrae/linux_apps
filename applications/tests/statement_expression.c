/**
 * Copyright (c) 2022 IotaHydrae
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <stdio.h>

#define min_t(type, x, y)\
            ({ type __x = (x); type __y = (y); __x < __y ? __x : __y;})

#define min(x, y) ({ \
    const typeof(x) _x = (x); \
    const typeof(y) _y = (y); \
    (void) (&_x == &_y); \
    _x < _y ? _x : _y; \
})

int main(int argc, char **argv)
{
    int a = 10;
    int b = 15;

    printf("%d\n", min_t(int, a, b));
    printf("%d\n", min(a, b));
    return 0;
}