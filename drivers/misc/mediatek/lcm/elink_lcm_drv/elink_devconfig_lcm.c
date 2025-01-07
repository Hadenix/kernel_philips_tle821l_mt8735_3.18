#include <linux/string.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/pinctrl/consumer.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <asm-generic/gpio.h>

#include "lcm_drv.h"
#include "ddp_irq.h"

#include <mach/items.h>
#include "elink_devconfig_lcm.h"
/* #include <mach/mt_gpio.h> */

#define LCM_TAG                  "[elink/lcm] "
#define LCM_FUN(f)               pr_err(APS_TAG"%s\n", __func__)
#define LCM_ERR(fmt, args...)    pr_err(APS_TAG"%s %d : "fmt, __func__, __LINE__, ##args)
#define LCM_LOG(fmt, args...)    pr_err(APS_TAG fmt, ##args)
#define LCM_DBG(fmt, args...)    pr_err(APS_TAG fmt, ##args)

struct device *lcm_device = NULL;
unsigned int PowerVolt;
char PowerName[64];
static struct regulator *lcm_vgp = NULL;
static struct device_node *lcm_node;


void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
	gpio_direction_output(GPIO, output);
	gpio_set_value(GPIO, output);
}

int lcd_power_en(unsigned int enabled)
{
	int ret = 0;
	unsigned int volt;
	unsigned int isenable;

	printk("[%s] enable <%d>\n", __func__, enabled);

	if (NULL == lcm_vgp)
		return 0;

	if(enabled) {
		/* set voltage */
		ret = regulator_set_voltage(lcm_vgp, PowerVolt, PowerVolt);
		if (ret != 0) {
			printk("[%s] LCM: lcm failed to set %s voltage=%d\n", __func__, PowerName, PowerVolt);
			return ret;
		}

		/* get voltage settings again */
		volt = regulator_get_voltage(lcm_vgp);
		if (volt == PowerVolt)
			printk("[%s]LCM: check regulator voltage=%d pass!\n", __func__, PowerVolt);
		else
			printk("[%s]LCM: check regulator voltage=%d fail! (voltage: %d)\n", __func__, PowerVolt, volt);

		ret = regulator_enable(lcm_vgp);
		if (ret != 0) {
			printk("[%s]LCM: Failed to enable %s: %d\n", __func__, PowerName, ret);
			return ret;
		}
	} else {
		/* disable regulator */
		isenable = regulator_is_enabled(lcm_vgp);

		printk("[%s]LCM: lcm query regulator enable status[0x%d]\n", __func__, isenable);

		if (isenable) {
			ret = regulator_disable(lcm_vgp);
			if (ret != 0) {
				printk("[%s]LCM: lcm failed to disable lcm_vgp: %d\n", __func__, ret);
				return ret;
			}
			/* verify */
			isenable = regulator_is_enabled(lcm_vgp);
			if (!isenable)
				printk("LCM: lcm regulator disable pass\n");
		}
	}

	return ret;
}

int init_lcm_gpio(char *namegpio, unsigned int *GPIO)
{

	*GPIO = of_get_named_gpio(lcm_node, namegpio, 0);
	printk("[%s] %s = 0x%x\n", __func__, namegpio, *GPIO);

	return 0;
}

int init_lcm_power(char *Power_name, unsigned int Power_volt)
{
	int ret = 0;
	struct regulator *lcm_vgp_ldo;

	printk("[%s] LCM: set [%s] voltage to %d\n", __func__, Power_name, Power_volt);
	if(lcm_vgp == NULL) {
		printk("[%s] first init <%s>.\n", __func__, Power_name);
		lcm_vgp_ldo = devm_regulator_get(lcm_device, Power_name);
		if (IS_ERR(lcm_vgp_ldo)) {
			ret = PTR_ERR(lcm_vgp_ldo);
			printk("[%s]failed to get reg-lcm <%s> LDO, %d\n", __func__, Power_name, ret);
			return ret;
		}
		PowerVolt = Power_volt;
		lcm_vgp = lcm_vgp_ldo;
		strcpy(PowerName, Power_name);	
	} else {
		printk("[%s] <%s> has already init.\n", __func__, Power_name);
	}


	return ret;
}


static int elink_lcm_probe(struct device *dev)
{
	struct device_node *node;

	printk("[%s]\n", __func__);
	lcm_device = dev;

//Elink_Matt@20161031 add for diffrent dts file
#if defined(CONFIG_ARCH_MT6735)
	node = of_find_compatible_node(NULL, NULL, "mediatek,mt6735-dispsys");
//	#error 1
#endif

#if defined(CONFIG_ARCH_MT6735M)
	node = of_find_compatible_node(NULL, NULL, "mediatek,lcm");
//	#error 2
#endif

	lcm_node = node;

	return 0;
}

static struct platform_device elink_lcm_device = {
	.name = "elink_lcm_items",
	.id = 0,
	.dev = {
		}
};

static const struct of_device_id elink_lcm_of_ids[] = {
	{.compatible = "mediatek,lcm",},
	{}
};

static struct platform_driver elink_lcm_driver = {
	.driver = {
		   .name = "elink_lcm_items",
		   .owner = THIS_MODULE,
		   .probe = elink_lcm_probe,
#ifdef CONFIG_OF
		   .of_match_table = elink_lcm_of_ids,
#endif
		   },
};

static int __init lcm_control_init(void)
{
	pr_notice("LCM: Register lcm device\n");
	if(platform_device_register(&elink_lcm_device)) {
		pr_err("LCM: failed to register disp device\n");
		return -ENODEV;
	}
	pr_notice("LCM: Register lcm driver\n");
	if (platform_driver_register(&elink_lcm_driver)) {
		pr_err("LCM: failed to register disp driver\n");
		return -ENODEV;
	}

	return 0;
}

static void __exit lcm_control_exit(void)
{
	platform_driver_unregister(&elink_lcm_driver);
	pr_notice("LCM: Unregister lcm driver done\n");
}

late_initcall(lcm_control_init);
module_exit(lcm_control_exit);
MODULE_DESCRIPTION("Display subsystem Driver");
MODULE_LICENSE("GPL");