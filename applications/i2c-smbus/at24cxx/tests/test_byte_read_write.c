#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

__s32 smbus_access(int file, char read_write, __u8 Waddr, int size, union i2c_smbus_data *data)
{
    i2c_smbus_ioctl_data msgs;
    __s32 ret;

    msgs.read_write = read_write;
    msgs.command = Waddr;
    msgs.size = size;
    msgs.data = data;

    if(ioctl(file, I2C_SMBUS, &msgs) < 0){
        perror("error, failed to access smbus");
        return -errno;
    }
}

__s32 write_byte_data(int file, __u32 Waddr, __u32 value)
{
    union i2c_smbus_data data;
    data.byte = value;
    return smbus_access(file,I2C_SMBUS_WRITE, Waddr, I2C_SMBUS_BYTE_DATA, &data);
}

__u32 random_read(int file, __u32 Waddr)
{
    union i2c_smbus_data data;
    __s32 ret;

    ret = smbus_access(file,I2C_SMBUS_READ, Waddr, I2C_SMBUS_BYTE_DATA, &data);
    if(ret < 0){
        return -1;
    }

    return data.byte;
}

int main(int argc, char const *argv[])
{
    int file;
    __s32 byte;

    file = open("/dev/i2c-0", O_RDWR);

    ioctl(file, I2C_SLAVE_FORCE, 0x50);

    write_byte_data(file, 0x00, 0x13);\
    byte = random_read(file, 0x00);

    printf("0x%x\n", byte);


    close(file);
    return 0;
}
