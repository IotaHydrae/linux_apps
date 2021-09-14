/*
	cdev_template.c - template for char device, char device interface

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
#include <linux/kernel.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/module.h>

/**
 * Replace CDEV_TEMPLATE or cdev_template to your device name.
 */

/* DEFINE */
/*#define CDEV_TEMPLATE_IOC_MAGIC		'd'
#define cdev_template_IOCINIT			_IOW(cdev_template_IOC_MAGIC, 0, int)
#define cdev_template_IOCWVALUE		_IOWR(cdev_template_IOC_MAGIC, 1, void *)*/
#define drv_inf(msg) printk(KERN_INFO "%s: "msg ,__func__)
#define drv_dbg(msg) printk(KERN_DEBUG "%s: "msg, __func__)
#define drv_wrn(msg) printk(KERN_WARNING "%s: "msg, __func__)
#define drv_err(msg) printk(KERN_ERR "%s: "msg, __func__)

/* Make your device type here */
struct cdev_template_device{
	/* e.g. unsigned char vgram[1024]; */
	struct cdev cdev;
};
struct cdev_template_device *cdev_template_devp;


/* STATIC */
static int cdev_ok;
static int cdev_template_nr_devs = 1;
static int cdev_template_major;
static int cdev_template_minor;
static dev_t cdev_template_devid;
static struct class *cdev_template_class;
static struct device *cdev_template_dev;


/* PROTOTYPE FOR DEVICE */
static int cdev_template_init(void);


/* PROTOTYPE FOR DRIVER */
static void cdev_template_cleanup(void);
static int cdev_template_setup_cdev(struct cdev *dev);
static int cdev_template_driver_open(struct inode *inode, struct file *filp);
static int cdev_template_driver_read(struct file *, char __user *, size_t, loff_t *);


/* cdev self operations start */
static int cdev_template_init(void)
{
	int ret=0;
	return ret;
}
/* cdev self operations end */


static void cdev_template_cleanup(void)
{
	if(cdev_template_device)
		device_destroy(cdev_template_class, MKDEV(cdev_template_major, 0));
	if(cdev_template_class)
		class_destroy(cdev_template_class);
	if(cdev_ok)
		cdev_del(bonbon_cdev);

	kfree(cdev_template_devp);
	unregister_chrdev_region(MKDEV(cdev_template_major, 0), 1);
}

static int cdev_template_setup_cdev(struct cdev *dev)
{
	int err;
	printk(KERN_INFO"%s Setting up char dev.\n",__func__);
	cdev_init(dev, &cdev_template_fops);
	dev->owner=THIS_MODULE;
	dev->ops = &cdev_template_fops;

	err = cdev_add(dev,cdev_template_devid,1);
	if(err){
		printk(KERN_INFO "%s: error on setup cdev : %d",__func__, err);
	}

	return err;
}


static struct file_operations cdev_template_fops = {
	.owner   = THIS_MODULE,
	.open    = cdev_template_driver_open,
	.read    = cdev_template_driver_read,
	/*.unlocked_ioctl   = cdev_template_driver_ioctl,*/
	.release = cdev_template_driver_release,
};


static int cdev_template_driver_open(struct inode *inode, struct file *filp)
{
	/*e.g. call cdev_template_init*/
	drv_dbg("openned.");
	return 0;
}

static ssize_t cdev_template_driver_read(struct file *, char __user *, size_t, loff_t *)
{
	drv_dbg("reading.");

	return 0;
}

static int cdev_template_driver_release (struct inode *, struct file *)
{
	drv_dbg("release.");

	return 0;
}

static __init int cdev_template_driver_init(void)
{
	int result;

	printk(KERN_INFO"%s driver initing.\n",__func__);

	/* Attempt to assign primary device number  */
	if(cdev_template_major){
		cdev_template_devid = MKDEV(cdev_template_major, cdev_template_minor);
		result = register_chrdev_region(cdev_template_devid, cdev_template_nr_devs, "cdev_template");
	}else{
		result = alloc_chrdev_region(&cdev_template_devid, 0, cdev_template_nr_devs, "cdev_template");
		cdev_template_major = MAJOR(cdev_template_devid);
	}

	/* As for now. No driver operation has been connected to these device numbers. */
	if(result<0){
		printk(KERN_WARNING "%s: can't get major %d\n",__func__, cdev_template_major);
		goto fail;
	}
	printk(KERN_INFO "%s: major %d.\n",__func__,cdev_template_major);

	cdev_template_devp = kmalloc(sizeof( struct cdev_template_device), GFP_KERNEL);
	if(!cdev_template_devp){
		result = -ENOMEM;
		goto fail;
	}
	memset(cdev_template_devp, 0, sizeof(struct cdev_template_device));
	
	result = cdev_template_setup_cdev(cdev_template_devp);
	
	cdev_template_class  = class_create(THIS_MODULE, "cdev_template");
	if(IS_ERR(cdev_template_class)){
		return PTR_ERR(cdev_template_class);
	}
	cdev_template_dev = device_create(cdev_template_class, NULL, 
							MKDEV(cdev_template_major, 0), NULL, "cdev_template");
	if(unlikely(IS_ERR(cdev_template_dev))){
		return PTR_ERR(cdev_template_dev);
	}
	
	if(!result){
		cdev_ok = 1;
	}else{
		goto fail;
	}

	return 0;

fail:
	cdev_template_cleanup();
	return result;

}

static __exit void cdev_template_driver_exit(void)
{
	printk(KERN_INFO "%s Exiting.\n", __func__);
	/*device_destroy(cdev_template_class, MKDEV(cdev_template_major, 0));
	class_destroy(cdev_template_class);
	cdev_del(bonbon_cdev);
	unregister_chrdev_region(MKDEV(cdev_template_major, 0), 1);*/
	cdev_template_cleanup();
}

module_init(cdev_template_driver_init);
module_exit(cdev_template_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zheng Hua <writeforever@foxmail.com>");
MODULE_DESCRIPTION("CDEV/ a driver template for cdev");
