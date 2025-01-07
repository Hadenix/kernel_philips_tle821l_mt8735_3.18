
#ifndef _ELINK_DEVCONFIG_LCM_H__
#define _ELINK_DEVCONFIG_LCM_H__

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

#define VOL_2800 2800000
#define VOL_1800 1800000
#define VOL_1500 1500000
#define VOL_1200 1200000
#define VOL_1000 1000000


int init_lcm_power(char *Power_name, unsigned int Power_volt);
int lcd_power_en(unsigned int enabled);
int init_lcm_gpio(char *namegpio, unsigned int *GPIO);
void lcm_set_gpio_output(unsigned int GPIO, unsigned int output);

#endif

