//SPDX-License-Identifier: GPL-2.0-or-later
/*
 * NS2009 touchscreen driver
 *
 * Copyright (C)
 *
 * Based on
 */
 
#include <linux/input.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/i2c.h>
#include <linux/regmap.h>

static const struct input_id ns2009_input_id = {
	.bustype = BUS_I2C,
	.product = 2009,
};

static int ns2009_cmd(struct device *dev, u8 cmd)
{
	return 0;
}

static int ns2009_probe(struct i2c_client *i2c,
			const struct i2c_device_id *id)
{
	return 0;
}

static int ns2009_remove(struct spi_device *spi)
{
	return 0;
}

static const struct i2c_device_id ns2009_idtable[] = {
	{ "ns2009", "0" },
	{ }
};

static const struct of_device_id ns2009_of_match[] = {
	{ .compatible = "ns2009" },
	{ }
};
MODULE_DEVICE_TABLE(of, ns2009_of_match);

static struct i2c_driver ns2009_driver = {
	.driver = {
		.name = "ns2009",
		.of_match_table = of_match_ptr(ns2009_of_match),
	},
	.id_table     = ns2009_idtable,
	.probe        = ns2009_probe,
	.remove       = ns2009_remove,
};
module_i2c_driver(ns2009_driver);

MODULE_AUTHOR("Zheng Hua <writeforever@foxmail.com>");
MODULE_DESCRIPTION("NS2009 Touchscreen Driver");
MODULE_LICENSE("GPL");
