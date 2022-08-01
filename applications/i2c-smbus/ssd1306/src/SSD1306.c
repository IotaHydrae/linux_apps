/**
 * @file SSD1306.c
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
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "SSD1306.h"

static __u8 oled_buffer[512] = {0};  /* a 512 byte size display buffer */

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
static int smbus_access(int fd, char read_write, __u8 addr, int size, union i2c_smbus_data *data)
{
    struct i2c_smbus_ioctl_data msgs;

    /* if i2c bus has not been openned, we don't need go further */
    if (fd < 0)
    {
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

    if (ioctl(fd, I2C_SMBUS, &msgs) < 0)
    {
        perror("error, failed to access smbus");
        return -errno;
    }

    return 0;
}

int ssd1306_write_cmd(int fd, __u8 val)
{
    union i2c_smbus_data data;

    data.byte = val;

    return smbus_access(fd, I2C_SMBUS_WRITE, SSD1306_CMD,
                        I2C_SMBUS_BYTE_DATA, &data);
}

int ssd1306_write_data(int fd, __u8 val)
{
    union i2c_smbus_data data;

    data.byte = val;

    return smbus_access(fd, I2C_SMBUS_WRITE, SSD1306_DATA,
                    I2C_SMBUS_BYTE_DATA, &data);
}

int ssd1306_set_addr_mode(int fd, int mode)
{
    /* trig mode setting */
    ssd1306_write_cmd(fd, 0x20);

    switch (mode)
    {
    case SSD1306_ADDR_MODE_HORIZONTAL:
        ssd1306_write_cmd(fd, 0x00);
        break;

    case SSD1306_ADDR_MODE_VERTICAL:
        ssd1306_write_cmd(fd, 0x01);
        break;

    case SSD1306_ADDR_MODE_PAGE:
        ssd1306_write_cmd(fd, 0x02);
        break;

    case SSD1306_ADDR_MODE_INVALID:
        ssd1306_write_cmd(fd, 0x03);
        break;
           
    default:
        ssd1306_write_cmd(fd, 0x02);
        break;
    }

    return 0;
}

void ssd1306_set_pos(int fd, __u8 page, __u8 col)
{
	ssd1306_write_cmd(fd, 0xb0 + page);

	ssd1306_write_cmd(fd, 0x00 | (col & 0x0f));
	ssd1306_write_cmd(fd, 0x10 | (col >> 4));
}

void oled_flush(int fd)
{
	__u8 page, col;

	for (page = 0; page < SSD1306_PAGE_SIZE; page++)
		for (col = 0; col < SSD1306_HOR_RES_MAX; col++)
		// if (oled_buffer[OFFSET(page, col)] != 0x00)
		{
			ssd1306_set_pos(fd, page, col);
			ssd1306_write_data(fd, oled_buffer[OFFSET(page, col)]);
		}
}

void oled_clear(int fd)
{
	__u8 page, col;

	for (page = 0; page < SSD1306_PAGE_SIZE; page++)
		for (col = 0; col < 128; col++)
			if (oled_buffer[OFFSET(page, col)] > 0x00)
			{
				ssd1306_set_pos(fd, page, col);
				ssd1306_write_data(fd, 0x00);
			}
	// memset(oled_buffer, 0x0, 1024);
	// oled_flush();
}

static void ssd1306_device_init(int fd)
{
	ssd1306_write_cmd(fd, 0xAE);//--display off	
	ssd1306_write_cmd(fd, 0x40);//---set low column address
	ssd1306_write_cmd(fd, 0xB0);//---set high column address
	ssd1306_write_cmd(fd, 0xC8);//-not offset
	ssd1306_write_cmd(fd, 0x81);// contract control
	ssd1306_write_cmd(fd, 0xFF);//--128 
	ssd1306_write_cmd(fd, 0xA1);//set segment remap 
	ssd1306_write_cmd(fd, 0xA6);//--normal / reverse	
	ssd1306_write_cmd(fd, 0xA8);//--set multiplex ratio(1 to 64)
	ssd1306_write_cmd(fd, 0x1F);	
	ssd1306_write_cmd(fd, 0xD3);//-set display offset
	ssd1306_write_cmd(fd, 0x00);
	ssd1306_write_cmd(fd, 0xD5);//set osc division
	ssd1306_write_cmd(fd, 0xF0);	
	ssd1306_write_cmd(fd, 0xD9);//Set Pre-Charge Period
	ssd1306_write_cmd(fd, 0x22);	
	ssd1306_write_cmd(fd, 0xDA);//set com pin configuartion
	ssd1306_write_cmd(fd, 0x02);	
	ssd1306_write_cmd(fd, 0xDB);//set Vcomh
	ssd1306_write_cmd(fd, 0x49);	
	ssd1306_write_cmd(fd, 0x8D);//set charge pump enable
	ssd1306_write_cmd(fd, 0x14);	
	ssd1306_write_cmd(fd, 0xAF);//--turn on oled panel

    ssd1306_set_addr_mode(fd, SSD1306_ADDR_MODE_PAGE);
}

void ssd1306_init(int fd)
{
	int count = 0;
	ssd1306_device_init(fd);

	// for (; count < SSD1306_BUFFER_SIZE; count++)
	// {
	// 	oled_buffer[count] = 0x00;
	// }
}

void oled_set_pixel(int fd, __u32 x, __u32 y, __u8 color)
{
	__u8 page, page_left;
	__u8 *pen = oled_buffer;

#ifdef OLED_COORD_CHECK
	if ((x >= 0 && x < SSD1306_HOR_RES_MAX) && (y >= 0 && y < SSD1306_VER_RES_MAX))
	{
#endif
		page = y / 8;
		page_left = y % 8 == 0 ? 0 : y % 8;
        // printf("page, page_left: %d, %d\n",page, page_left);

		if (color)
		{
			pen[OFFSET(page, x)] |= (1 << page_left);
            printf("+++ pos : %d, dump: %d\n", OFFSET(page, x), pen[OFFSET(page, x)]);
		}
		else
		{
			pen[OFFSET(page, x)] &= ~(1 << page_left);
            printf("--- pos : %d, dump: %d\n", OFFSET(page, x), pen[OFFSET(page, x)]);
		}

#ifdef OLED_COORD_CHECK
	}
#endif

	/*oled_set_pos(page, x);
	oled_write_dat(oled_buffer[offset]);*/
}