/**
 * @file main.c
 * @author IotaHydrae (writeforever@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2022-08-01
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

#include "include/SSD1306.h"

int main(int argc, char **argv)
{
    int fd;
    int rc;

    fd = open("/dev/i2c-1", O_RDWR);
    if (fd < 0)
    {
        fprintf(stderr, "failed to open i2c bus\n");
        return -1;
    }

    rc = ioctl(fd, I2C_SLAVE_FORCE, SSD1306_ADDRESS);
    if (rc < 0)
    {
        fprintf(stderr, "failed to set slave addr\n");
        return -1;
    }

    ssd1306_init(fd);
#if 1
    while (1)
    {
        for (int x = 0; x < 128; x++)
        {
            for (int y = 0; y < 32; y++)
            {
                oled_set_pixel(fd, x, y, 0xff);
            }
        }
        oled_flush(fd);
        sleep(1);

        for (int x = 0; x < 128; x++)
        {
            for (int y = 0; y < 32; y++)
            {
                oled_set_pixel(fd, x, y, 0x00);
            }
        }
        oled_flush(fd);
        sleep(1);
    }
#else
    for (int x = 0; x < 128; x++)
    {
        for (int y = 0; y < 32; y++)
        {
            ssd1306_write_data(fd, 0xff);
        }
    }  
    while(1);
#endif

    close(fd);

    return 0;
}