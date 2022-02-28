#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "at24cxx.h"

static struct at24cxx_handle handle;

AT24CXX::AT24CXX(__u32 bus, __u32 addr)
{

    sprintf(handle.bus_path, "/dev/i2c-%d", bus);
    printf("%s\n", handle.bus_path);
    printf("%d\n", handle.bus_file);

    if ((handle.bus_file = open(handle.bus_path, O_RDWR) < 0))
    {
        fprintf(stderr, "error, cannot open i2c bus: %d", bus);
        handle.bus_file = -1;
        exit(1);
    }

    handle.bus_number = bus;
    handle.device_addr = addr;

    printf("%d, %d, %d\n",handle.bus_file, bus , addr);

    if(ioctl(handle.bus_file, I2C_SLAVE, handle.device_addr) < 0){
        perror("error, failed to set deivce address!");
        // exit(1);
    }
}

AT24CXX::~AT24CXX()
{
    close(handle.bus_file);
}

__s32 AT24CXX::detect()
{
    unsigned long funcs= 0x00;

    printf("checking functions...\n");
    if (handle.bus_file < 0)
    {
        printf("error, Bus node have not been open.\n");
        return -1;
    }

    if (ioctl(handle.bus_file, I2C_FUNCS, &funcs) < 0)
    {
        perror("error, ioctl failed");
        return -1;
    }

    printf("0x%x\n", funcs);

    if (!(funcs & I2C_FUNC_SMBUS_WRITE_BYTE_DATA))
    {
        printf("Oops, the needed functionality (SMBus write_quick function) is not available!\n");
        return -1;
    }
}

/*
//This is the structure as used in the I2C_SMBUS ioctl call
struct i2c_smbus_ioctl_data {
	__u8 read_write;
	__u8 command;
	__u32 size;
	union i2c_smbus_data *data;
};
*/
__s32 AT24CXX::access(char read_write, __u8 Waddr, int size, union i2c_smbus_data *data)
{
    i2c_smbus_ioctl_data msgs;
    __s32 ret;

    msgs.read_write = read_write;
    msgs.command = Waddr;
    msgs.size = size;
    msgs.data = data;

    if(ioctl(handle.bus_file, I2C_SMBUS, &msgs) < 0){
        perror("error, failed to access smbus");
        return -errno;
    }
}

__s32 AT24CXX::write_byte_data(__u32 Waddr, __u32 value)
{
    union i2c_smbus_data data;
    data.byte = value;
    return access(I2C_SMBUS_WRITE, Waddr, I2C_SMBUS_BYTE_DATA, &data);
}

__u32 AT24CXX::random_read(__u32 Waddr)
{
    union i2c_smbus_data data;
    __s32 ret;

    ret = access(I2C_SMBUS_READ, Waddr, I2C_SMBUS_BYTE_DATA, &data);
    if(ret < 0){
        return -1;
    }

    return data.byte;
}