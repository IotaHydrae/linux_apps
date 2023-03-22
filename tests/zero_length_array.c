/**
 * Copyright (c) 2022 IotaHydrae
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <stdio.h>

struct var_data{
    int len;
    char data[0];
};

int main(int argc, char **argv)
{
    int a = 20;
    struct var_data s;
    double x[a];

    s.len = 20;

    for(int i=0;i<s.len;i++)
        printf("%02x, %p\n", s.data[i], &s.data[i]);

    for(int i=0;i<a;i++)
        x[i] = i;
    
    for(int i=0;i<a;i++)
        printf("%f, %p\n", x[i], &x[i]);

    return 0;
}