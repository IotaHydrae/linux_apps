
#include <stdio.h>

#include "at24cxx.h"




int main(int argc, char const *argv[])
{
	//static int file;
    __u8 byte = 0;
/*
    file = open("/dev/i2c-0", O_RDWR);
	if(file < 0){
		fprintf(stderr, "failed to open i2c bus\n");
		return -1;
	}
*/
	AT24CXX at24cxx = AT24CXX("/dev/i2c-0", 0x50);

	at24cxx.write_byte_data(0x00, 0x13);
	at24cxx.waiting_write_cycle();
	byte = at24cxx.random_read(0x00);

	printf("0x%x\n", byte);

	//close(file);
	return 0;
}
