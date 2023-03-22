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

enum pcf8574_pins {
    PCF8574_P0,
    PCF8574_P1,
    PCF8574_P2,
    PCF8574_P3,
    PCF8574_P4,
    PCF8574_P5,
    PCF8574_P6,
    PCF8574_P7,
};

enum pcf8574_pin_state {
    PCF8574_PIN_LOW,
    PCF8574_PIN_HIGH,
};

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
static int __smbus_access(int fd, __u8 rw, __u8 cmd, __u32 size, union i2c_smbus_data *data)
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
static int __pcf8574_write_byte(int fd, __u8 byte)
{
    return __smbus_access(fd, I2C_SMBUS_WRITE, byte, I2C_SMBUS_BYTE, NULL);
}

static int __pcf8574_read_byte(int fd)
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
    printf("raw data: %d\n", data.byte);

    return data.byte;
}

int pcf8574_gpio_set(int fd, __u8 num, __u8 val)
{
    __u8 dump_val = __pcf8574_read_byte(fd);
    
    if (val)
        dump_val |= (num << val);
    else
        dump_val &= ~(num << val);
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