#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

static int smbus_access(int fd, char read_write, __u8 Waddr, int size, union i2c_smbus_data *data)
{
    struct i2c_smbus_ioctl_data msgs;

    msgs.read_write = read_write;
    msgs.command = Waddr;
    msgs.size = size;
    msgs.data = data;

    if(ioctl(fd, I2C_SMBUS, &msgs) < 0){
        perror("error, failed to access smbus");
        return -errno;
    }

    return 0;
}

int main(int argc, char **argv)
{
    int fd;
    int rc;

    fd = open("/dev/i2c-1", O_RDWR);
    if(fd < 0) {
        fprintf(stderr, "failed to open i2c bus\n");
        return -1;
    }

    rc = ioctl(fd, I2C_SLAVE_FORCE, 0x57);
    if(rc < 0) {
        fprintf(stderr, "failed to set slave addr\n");
        return -1;
    }

    union i2c_smbus_data data;
    smbus_access(fd, I2C_SMBUS_READ, 0xff, I2C_SMBUS_BYTE_DATA, &data);
    printf("Read : 0x%x\n", data.byte);

    return 0;
}