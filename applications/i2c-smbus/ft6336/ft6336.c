/**
 * @file ft6336.c
 * @author IotaHydrae (writeforever@foxmail.com)
 * @brief FocalTech FT6336U Self-Capacitive Touch Panel Controller
 * @version 0.1
 * @date 2023-01-10
 *
 * MIT License
 *
 * Copyright 2022 IotaHydrae(writeforever@foxmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *
 */

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

#define FT6336_ADDRESS 0x38

#define FT6336_REG_DEV_MODE     0x00
#define FT6336_REG_GEST_ID      0x01
#define FT6336_REG_TD_STATUS    0x02

#define FT6336_REG_P1_XH        0x03
#define FT6336_REG_P1_XL        0x04
#define FT6336_REG_P1_YH        0x05
#define FT6336_REG_P1_YL        0x06
#define FT6336_REG_P1_WEIGHT    0x07
#define FT6336_REG_P1_MISC      0x08

#define FT6336_REG_P2_XH        0x09
#define FT6336_REG_P2_XL        0x0a
#define FT6336_REG_P2_YH        0x0b
#define FT6336_REG_P2_YL        0x0c
#define FT6336_REG_P2_WEIGHT    0x0d
#define FT6336_REG_P2_MISC      0x0e

#define FT6336_REG_TH_GROUP     0x80
#define FT6336_REG_TH_DIFF      0x85
#define FT6336_REG_CTRL         0x86
#define FT6336_REG_TIMEENTERMONITOR 0x87
#define FT6336_REG_PERIODACTIVE 0x88
#define FT6336_REG_PERIODMONITOR    0x89
#define FT6336_REG_RADIAN_VALUE 0x91
#define FT6336_REG_OFFSET_LEFT_RIGHT    0x92
#define FT6336_REG_OFFSET_UP_DOWN   0x93
#define FT6336_REG_DISTANCE_LEFT_RIGHT  0x94
#define FT6336_REG_DISTANCE_UP_DOWN 0x95
#define FT6336_REG_DISTANCE_ZOOM    0x96

#define FT6336_REG_LIB_VER_H    0xa1
#define FT6336_REG_LIB_VER_L    0xa2
#define FT6336_REG_CIPHER       0xa3
#define FT6336_REG_G_MODE       0xa4
#define FT6336_REG_PWR_MODE     0xa5
#define FT6336_REG_FIRMWARE_ID  0xa6
#define FT6336_REG_FOCALTECH_ID 0xa8
#define FT6336_REG_RELEASE_CODE_ID  0xaf
#define FT6336_REG_STATE        0xbc

int g_fd_i2c_bus;

/**
 * @brief easy smbus access
 *
 * @param fd fd of i2c/smbus
 * @param read_write direction of operation
 * @param addr register addr of device
 * @param size how many bytes of this operation
 * @param data recv or send data
 * @return int return -errno on error, 0 on success
 */
static int smbus_access(int fd, char read_write, __u8 addr, int size,
                        union i2c_smbus_data *data)
{
    struct i2c_smbus_ioctl_data msgs;
    
    /* if i2c bus has not been openned, we don't need go further */
    if (fd < 0) {
        printf("i2c/smbus has not been openned!");
        return -1;
    }
    
    /* The read_write will be I2C_SMBUS_READ | I2C_SMBUS_WRTIE
     * the addr will be any add of device
     * the size will be I2C_SMBUS_BYTE | I2C_SMBUS_BYTE_DATA ..
     * te data will be used to IO data
     */
    msgs.read_write = read_write;
    msgs.command = addr;
    msgs.size = size;
    msgs.data = data;
    
    if (ioctl(fd, I2C_SMBUS, &msgs) < 0) {
        perror("error, failed to access smbus");
        return -errno;
    }
    
    return 0;
}

__u8 i2c_read_reg(__u8 addr, __u8 reg)
{
    int rc;
    union i2c_smbus_data data;
    
    rc = smbus_access(g_fd_i2c_bus, I2C_SMBUS_READ, reg,
                      I2C_SMBUS_BYTE_DATA, &data);
    if (rc < 0) {
        printf("i2c transfer error!\n");
    }
    return data.byte;
}

void i2c_write_reg(__u8 addr, __u8 reg, __u8 val)
{
    union i2c_smbus_data data;
    
    data.byte = val;
    
    smbus_access(g_fd_i2c_bus, I2C_SMBUS_WRITE, reg,
                 I2C_SMBUS_BYTE_DATA, &data);
}

__u8 ft6336_read_firmware_id(void)
{
    return i2c_read_reg(FT6336_ADDRESS, FT6336_REG_FIRMWARE_ID);
}

__u8 ft6336_read_release_code(void)
{
    return i2c_read_reg(FT6336_ADDRESS, FT6336_REG_RELEASE_CODE_ID);
}

__u8 ft6336_read_focaltech_id(void)
{
    return i2c_read_reg(FT6336_ADDRESS, FT6336_REG_FOCALTECH_ID);
}

__u8 ft6336_read_device_mode(void)
{
    return i2c_read_reg(FT6336_ADDRESS, FT6336_REG_DEV_MODE);
}

void ft6336_set_device_mode(__u8 mode)
{
    i2c_write_reg(FT6336_ADDRESS, FT6336_REG_DEV_MODE, mode);
}

__u8 ft6336_read_gest_id(void)
{
    return i2c_read_reg(FT6336_ADDRESS, FT6336_REG_GEST_ID);
}

__u8 ft6336_read_td_status(void)
{
    return i2c_read_reg(FT6336_ADDRESS, FT6336_REG_TD_STATUS);
}

void ft6x36_set_threshold_group(__u8 threshold)
{
    i2c_write_reg(FT6336_ADDRESS, FT6336_REG_TH_GROUP, threshold);
}

void ft6x36_set_period_active(__u8 rate)
{
    i2c_write_reg(FT6336_ADDRESS, FT6336_REG_PERIODACTIVE, rate);
}

int main(int argc, char **argv)
{
    int rc;
    
    g_fd_i2c_bus = open("/dev/i2c-1", O_RDWR);
    if (g_fd_i2c_bus < 0) {
        fprintf(stderr, "failed to open i2c bus\n");
        return -1;
    }
    
    rc = ioctl(g_fd_i2c_bus, I2C_SLAVE_FORCE, FT6336_ADDRESS);
    if (rc < 0) {
        fprintf(stderr, "failed to set slave addr\n");
        return -1;
    }
    
    __u8 val = ft6336_read_firmware_id();
    printf("firmware id : %d\n", val);
    
    val = ft6336_read_release_code();
    printf("release code id : %d\n", val);
    
    val = ft6336_read_focaltech_id();
    printf("focaltech id : %d\n", val);
    
    val = ft6336_read_device_mode();
    printf("device mode : %d\n", val);
    
    ft6336_set_device_mode(0x00);
    ft6x36_set_threshold_group(0x16);
    ft6x36_set_period_active(0x0e);
    
    while (1) {
        val = ft6336_read_td_status();
        // printf("point count : %d\n", val);
        if (val >= 1) {
            val = ft6336_read_gest_id();
            switch (val) {
            case 0x10:
                printf("move up\n");
                break;
            case 0x14:
                printf("move right\n");
                break;
            case 0x18:
                printf("move down\n");
                break;
            case 0x1c:
                printf("move left\n");
                break;
            case 0x48:
                printf("zoom in\n");
                break;
            case 0x49:
                printf("zoom out\n");
                break;
            case 0x00:
                // printf("no gesture\n");
                break;
            default:
                printf("error\n");
                break;
            }
        }
        // usleep(500 * 1000);
    }
    
    return 0;
}