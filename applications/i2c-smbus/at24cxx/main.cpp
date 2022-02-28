#include <stdio.h>
#include "at24cxx.h"

int main(int argc, char const *argv[])
{
    __u8 byte = 0;

    AT24CXX at24cxx = AT24CXX(0, 0x50);

    at24cxx.write_byte_data(0x00, 0x13);
    byte = at24cxx.random_read(0x00);

    printf("%d\n", byte);

    return 0;
}
