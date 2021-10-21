/*
	demo_wait_queue.c - a test driver for wait queue.

	Copyright (C) 2021 Zheng Hua <writeforever@foxmail.com>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h> 	/* for kmalloc */
#include <linux/kernel.h>	/* container_of */
#include <linux/list.h>	/* for list */
#include <linux/wait.h>	/* for wait queue */

#define drv_inf(msg) printk(KERN_INFO "%s: "msg ,__func__)
#define drv_dbg(msg) printk(KERN_DEBUG "%s: "msg, __func__)
#define drv_wrn(msg) printk(KERN_WARNING "%s: "msg, __func__)
#define drv_err(msg) printk(KERN_ERR "%s: "msg, __func__)

#define TYPE_STATIC

#ifdef TYPE_STATIC
#undef TYPE_DYNAMIC
DECLARE_WAIT_QUEUE_HEAD
#endif

static __init int demo_init(void)
{
#ifdef TYPE_DYNAMIC
#undef TYPE_STATIC

#endif

    return 0;
}

static __exit void demo_exit(void)
{
    printk(KERN_INFO "%s Exiting.\n", __func__);

}

module_init(demo_init);
module_exit(demo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zheng Hua <writeforever@foxmail.com>");
MODULE_DESCRIPTION("CDEV/ a test driver for wait queue");
