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
    struct var_data s;

    for(int i=0;i<s.len;i++)
        printf("%02x", s.data[i]);

    return 0;
}