#include <linux/err.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/slab.h>

#define LRADC_CTRL		0x00
#define LRADC_INTC		0x04
#define LRADC_INTS		0x08
#define LRADC_DATA0		0x0c
#define LRADC_DATA1		0x10


/* LRADC_CTRL bits */
#define FIRST_CONVERT_DLY(x)	((x) << 24) /* 8 bits */
#define CHAN_SELECT(x)		((x) << 22) /* 2 bits */
#define CONTINUE_TIME_SEL(x)	((x) << 16) /* 4 bits */
#define KEY_MODE_SEL(x)		((x) << 12) /* 2 bits */
#define LEVELA_B_CNT(x)		((x) << 8)  /* 4 bits */
#define HOLD_KEY_EN(x)		((x) << 7)
#define HOLD_EN(x)		((x) << 6)
#define LEVELB_VOL(x)		((x) << 4)  /* 2 bits */
#define SAMPLE_RATE(x)		((x) << 2)  /* 2 bits */
#define ENABLE(x)		((x) << 0)

/* LRADC_INTC and LRADC_INTS bits */
#define CHAN1_KEYUP_IRQ		BIT(12)
#define CHAN1_ALRDY_HOLD_IRQ	BIT(11)
#define CHAN1_HOLD_IRQ		BIT(10)
#define	CHAN1_KEYDOWN_IRQ	BIT(9)
#define CHAN1_DATA_IRQ		BIT(8)
#define CHAN0_KEYUP_IRQ		BIT(4)
#define CHAN0_ALRDY_HOLD_IRQ	BIT(3)
#define CHAN0_HOLD_IRQ		BIT(2)
#define	CHAN0_KEYDOWN_IRQ	BIT(1)
#define CHAN0_DATA_IRQ		BIT(0)

struct suniv_f1c100s_lradc_dev {
	
};

static int suniv_f1c100s_lradc_probe(struct platform_device *pdev)
{
	printk();
	return 0;
}


static const struct of_device_id suniv_f1c100s_lradc_of_match[] = {
	{ .compatible = "allwinner,sun4i-a10-lradc-keys" },
};
MODULE_DEVICE_TABLE(of, suniv_f1c100s_lradc_of_match);

static struct platform_driver suniv_f1c100s_lradc_driver = {
	.driver = {
		.name	= "suniv_f1c100s_lradc-keys",
		.of_match_table = of_match_ptr(suniv_f1c100s_lradc_of_match),
	},
	.probe	= suniv_f1c100s_lradc_probe,
};

module_platform_driver(suniv_f1c100s_lradc_driver);

MODULE_DESCRIPTION("Allwinner suniv f1c100s low res adc test driver");
MODULE_AUTHOR("Hans de Goede <hdegoede@redhat.com>");
MODULE_AUTHOR("Zheng Hua <writeforever@foxmail.com>");
MODULE_LICENSE("GPL");
