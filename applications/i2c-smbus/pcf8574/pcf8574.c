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

/**
 * @brief used to access the i2c bus
 * 
 * @param fd file descriptor of the i2c bus
 * @param rw read or write
 * @param cmd the command to send
 * @param size size of total bytes to read or write
 * @param data data to send or read
 * @return return 0 on success, -errno on error
 */
static int smbus_access(int fd, __u8 rw, __u8 cmd, __u32 size, union i2c_smbus_data *data)
{
    struct i2c_smbus_ioctl_data msgs;

    msgs.read_write = rw;
    msgs.command = cmd;
    msgs.size = size;
    msgs.data = data;

    if(ioctl(fd, I2C_SMBUS, &msgs) < 0){
        perror("error, failed to access smbus");
        return -errno;
    }

    return 0;
}

/**
 * @brief write a command to the pcf8574
 * 
 * @param fd file descriptor of the i2c bus
 * @param cmd the command to send
 * @return return sumbus_access() call result
 */
int pcf8574_write_cmd(int fd, __u8 cmd)
{
    return smbus_access(fd, I2C_SMBUS_WRITE, cmd, I2C_SMBUS_BYTE, NULL);
}

int pcf8574_read_byte(int fd)
{
    union i2c_smbus_data data;
    struct i2c_smbus_ioctl_data msg;

    msg.read_write = I2C_SMBUS_READ;
    msg.size = I2C_SMBUS_BYTE_DATA;
    msg.data = &data;

    if(ioctl(fd, I2C_SMBUS, &msg) < 0){
        perror("error, failed to access smbus");
        return -errno;
    }
    printf("raw data: %d\n", data.word);

    return data.word;
}

int main(int argc, char **argv)
{
    int fd;
    int rc;

    /* open i2c bus */
    fd = open("/dev/i2c-1", O_RDWR);
	if(fd < 0){
		fprintf(stderr, "failed to open i2c bus\n");
		return -1;
	}

    /* set slave deivce addr */
    rc = ioctl(fd, I2C_SLAVE_FORCE, 0x27);
	if(rc < 0){
		fprintf(stderr, "failed to set slave addr\n");
		return -1;
	}
	
    while(1) {
        do {
            break;
        } while (0);
    }

    close(fd);

    return 0;
}