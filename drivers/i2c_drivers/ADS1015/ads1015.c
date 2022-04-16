#include <linux/kernel.h>
#include <linux/module.h>

struct ads1015_data {
	struct i2c_client *client;
	
};

static int 
ads1015_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct ads1015_data *ads1015;

	ads1015 = devm_kzalloc(&client->dev, sizeof(struct ads1015_data),
			GFP_KERNEL);
	if(!ads1015)
		return -ENOMEM;

	ads1015->client = client;
	i2c_set_clientdata(client, ads1015);

	return 0;
}

static int
ads1015_remove(struct i2c_client *client)
{
	return 0;
}

static const struct of_device_id ads1015_of_match_table[] = {
	{.compatible = "ti,ads1015"},
	{},
}

static struct i2c_driver ads1015_i2c_driver = {
	.probe = ads1015_probe,
	.remove = ads1015_remove, 	
};

MODULE_AUTHOR("IotaHydrae writeforever@foxmail.com");
MODULE_DESCRIPTION("a i2c driver for ads1015");
MODULE_LICENSE("GPL");
