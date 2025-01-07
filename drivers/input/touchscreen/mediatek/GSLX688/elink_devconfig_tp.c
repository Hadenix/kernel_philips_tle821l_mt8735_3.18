/******************************************************************************
 * elink_devconfig_tp.c - Elink Android Linux Touch Panel Device Driver       *
 *                                                                            *
 * Copyright 2016 Elinktek Co.,Ltd.                                           *
 *                                                                            *
 * DESCRIPTION:                                                               *
 *     this file provide redefine GPIO and pmic control in items.             *
 *                                                                            *
 * AUTHOR:                                                                    *
 *     Yin.Gen                                                                *
 *                                                                            *
 ******************************************************************************/

#include "tpd.h"
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/fb.h>

#ifdef CONFIG_COMPAT
#include <linux/compat.h>
#endif

#include <mach/items.h>
#include "elink_devconfig_tp.h"
#include "elink_tp_list.h"

#include <linux/time.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

#include <mach/items.h>

int touchpanel_gsl_ic = 0;

static unsigned int GSL_TP_ID_USED = 0;
static int touchpanel_gsl_modle = 0;
static u8 light_sleep_flag = 0;
struct timer_list  led_timer; 
struct device *tp_device = NULL;

//char name_gpio[64] = "gpio";
//char name_out0[64] = "_out0";
//char name_out1[64] = "_out1";
//char name_eint[64] = "_eint";

int sf_hwrotation = 0;
int xy_swap = 0;
int x_reverse= 0;
int y_reverse = 0;
int xy_deal = 0;
int xy_deal_minor = 0;

#define STRSPLICE(dest, a, b, c) \
	do{ \
		strcpy(dest, a); \
		strcat(dest, b); \
		strcat(dest, c); \
	} while(0)

static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
#if 0
    mt_set_gpio_mode(GPIO, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO, (output>0)? GPIO_OUT_ONE: GPIO_OUT_ZERO);
#endif
}

static void timer_func(unsigned long data)
{
	if(light_sleep_flag==1)
	{
		light_sleep_flag=0;

		if(LED_PIN) 
			lcm_set_gpio_output((LED_PIN | 0x80000000), 0);

	}	  
}

int tpd_hwPowerControl(unsigned int powerVolt, char *name, bool on)
{
	int ret = 0;

	printk("[%s] [%s] %s power.\n", __func__, name, on?"open":"off");
	if(on) {
		ret=regulator_set_voltage(tpd->reg, powerVolt, powerVolt);
		if (ret)
			printk("[%s] [%s] regulator_set_voltage() failed!\n", __func__, name);

		ret=regulator_enable(tpd->reg);  //enable regulator
		if (ret)
			printk("[%s] [%s] regulator_enable() failed!\n", __func__, name);
	} else {
		if (regulator_is_enabled(tpd->reg) != 0) {
			printk("[%s] [%s] is enabled, begin to close.\n",__func__ , name);
		}
		if ((ret = regulator_disable(tpd->reg)) != 0) {
			printk("[%s]fail to regulator_disable, powertype: %s \n\n",__func__, name);
			return ret;
		}
	}

	return ret;
}

int control_input_orientation(int *x, int *y, int screenwidth, int screenheight)
{
	int tpd_x_res=0, tpd_y_res=0;
	int z;

	if(90 == sf_hwrotation || 270 == sf_hwrotation)
	{	
		tpd_y_res = screenwidth;
		tpd_x_res = screenheight;
	} else {
		tpd_x_res = screenwidth;
		tpd_y_res = screenheight;
	}

	if(1 == xy_swap)
	{	
		z=*y; *y=*x; *x=z;
	}

	if(1 == x_reverse)
		*x=tpd_x_res-*x;

	if(1 == y_reverse)	
		*y=tpd_y_res-*y;

	if(1 == xy_deal)
	{
		*x=(*x)*tpd_y_res/tpd_x_res;
		*y=(*y)*tpd_x_res/tpd_y_res; 
	}

	if(1 == xy_deal_minor)
	{
		*x=(*x)*tpd_x_res/tpd_y_res;
		*y=(*y)*tpd_y_res/tpd_x_res; 
	}

	return 0;
}

int key_light_mod_timer(void)
{
	if(LED_PIN)
		mod_timer(&led_timer, jiffies + 300/(1000/HZ));

	return 0;
}

int key_light_init_timer(void)
{
	init_timer(&led_timer);
	led_timer.expires	= jiffies + 1/(1000/HZ);
	led_timer.function	= timer_func;

	return 0;
}

int control_key_light(int x, int y)
{
	int kx[4], ky;
   	int i;

   	//printk("[%s] in.\n", __func__);

	if(KEY_NUM && LED_PIN) {
   		for(i = 0; i < KEY_NUM; i++)
   		{
   			kx[i] = PTR_TPD_KEYS_DIM_LOCAL[i][0];   		
   		}	
		
		ky = PTR_TPD_KEYS_DIM_LOCAL[0][1];
	}

	if(LED_PIN) {
		switch(KEY_NUM)
		{
			case 0:
				break;
			case 1:
				if( x == kx[0] && y == ky ) {
					lcm_set_gpio_output( (LED_PIN | 0x80000000), 1 );
					light_sleep_flag=1;
				}
				break;
			case 2:
				if( (x == kx[0] || x == kx[1]) && y == ky ) {
					lcm_set_gpio_output( (LED_PIN | 0x80000000), 1 );
					light_sleep_flag=1;
				}
				break;
			case 3:
				if( (x == kx[0] || x == kx[1] || x == kx[2]) && y == ky ) {
					lcm_set_gpio_output( (LED_PIN | 0x80000000), 1 );
					light_sleep_flag=1;
				}
				break;
			default:
				break;
		}
	}

	return 0;
}

int match_gsl_tp_button(void)
{

	switch(KEY_NUM)
	{
		case 0:
			break;
		case 1:
			tpd_button_setting(KEY_NUM, tpd_keys_local_home, PTR_TPD_KEYS_DIM_LOCAL);
			break;
		case 3:
			tpd_button_setting(KEY_NUM, tpd_keys_local, PTR_TPD_KEYS_DIM_LOCAL);
			break;
		default:
			break;
	}

	return 0;

}

int match_gsl_tp_id(char *read_buf)
{
	int i,j;
	u8 temp[4]={0};

	printk("[%s] GSL_TP_ID_USED = %d, name_tp_mode = %s.\n", __func__, GSL_TP_ID_USED, name_tp_mode);

	if(GSL_TP_ID_USED) {
		for(i=0;i<TP_NUM;i++) { 
			
			printk("\n[%s] ID  %x %x %x %x \n", __func__, \
				PTR_TP_PROJECT_DATA[i].tp_id[3], \
				PTR_TP_PROJECT_DATA[i].tp_id[2], \
				PTR_TP_PROJECT_DATA[i].tp_id[1], \
				PTR_TP_PROJECT_DATA[i].tp_id[0]);

			for(j=0;j<4;j++) {
				temp[j]=PTR_TP_PROJECT_DATA[i].tp_id[j];
			}

			if( ( read_buf[3] == temp[0] ) && \
				( read_buf[2] == temp[1] ) && \
				( read_buf[1] == temp[2] ) && \
				( read_buf[0] == temp[3] ) ) { 
				PTR_FW = PTR_TP_PROJECT_DATA[i].p_fw;
				PTR_GSL_CONFIG_DATA_ID = PTR_TP_PROJECT_DATA[i].p_gsl_config_data_id;
				PTR_TPD_KEYS_DIM_LOCAL = PTR_TP_PROJECT_DATA[i].tpd_keys;
				TP_SOURCE_LEN = PTR_TP_PROJECT_DATA[i].tp_source_len;
				NAME_TP = PTR_TP_PROJECT_DATA[i].name;					
				GSL_TP_ID_USED=i; 
				break; 

			}
		}

		if( i==TP_NUM ) {
			PTR_FW = PTR_TP_PROJECT_DATA[i-1].p_fw;
			PTR_GSL_CONFIG_DATA_ID = PTR_TP_PROJECT_DATA[i-1].p_gsl_config_data_id;
			PTR_TPD_KEYS_DIM_LOCAL = PTR_TP_PROJECT_DATA[i-1].tpd_keys;
			TP_SOURCE_LEN = PTR_TP_PROJECT_DATA[i-1].tp_source_len;
			NAME_TP = PTR_TP_PROJECT_DATA[i-1].name;				
			GSL_TP_ID_USED=i-1;
		}	


	}
	return 0;
}

int tp_list_config(void)
{
    int i;
    int sizeoflist;

	sizeoflist = sizeof(gsl_tp_info) / sizeof(ALL_TP_INFO);
	for (i = 0; i < sizeoflist; i++)
	{
		if(strcmp(name_tp_mode, gsl_tp_info[i].name) == 0)
	    	break;
	}

	printk("\n[%s] name_tp_mode =%s\n",__func__, name_tp_mode);
	if (i == sizeoflist)
	{
		printk("\n[%s] list of designated tp isn't exist name_tp_mode =%s\n",__func__, name_tp_mode);
		return -1;
	}

	touchpanel_gsl_modle = gsl_tp_info[i].tp_gsl_modle;
	PTR_TP_PROJECT_DATA = gsl_tp_info[i].pro_tp_data;
	KEY_NUM = gsl_tp_info[i].HAVE_KEY;
	TP_NUM = gsl_tp_info[i].tp_num;
	LED_PIN = gsl_tp_info[i].gpio_led_pin;

	if( 1 != TP_NUM && 0 != TP_NUM) {
		if( KEY_NUM ) { 
			PTR_FW = GSL_TEST_HAVE_KEY_FW; 
			TP_SOURCE_LEN = ARRAY_SIZE(GSL_TEST_HAVE_KEY_FW);
		} else { 
			PTR_FW = GSL_TEST_FW; 
			TP_SOURCE_LEN = ARRAY_SIZE(GSL_TEST_FW);
		}
		GSL_TP_ID_USED = 0xff;    
	} else if( 1 == TP_NUM ) {
		if( KEY_NUM )   { PTR_TPD_KEYS_DIM_LOCAL = PTR_TP_PROJECT_DATA[0].tpd_keys; } 
		PTR_GSL_CONFIG_DATA_ID = PTR_TP_PROJECT_DATA[0].p_gsl_config_data_id;
		PTR_FW = PTR_TP_PROJECT_DATA[0].p_fw;
		TP_SOURCE_LEN = PTR_TP_PROJECT_DATA[0].tp_source_len;
		GSL_TP_ID_USED = 0;

	} else {
      printk("\n[%s] TP_NUM = %d\n",__func__, TP_NUM);
      return 0;
   }

   return 0;
}

int _tp_gpio_config(char *items_key)
{
	char STR_GPIO[64];
	char DTS_GPIO_OUT0[64];
	char DTS_GPIO_OUT1[64];
	char DTS_GPIO_EINT[64];
	int ret = 0;

	if(item_exist(items_key)) {
		item_string(STR_GPIO, items_key, 0);
		STRSPLICE(DTS_GPIO_OUT0, "gpio", STR_GPIO, "_out0");
		STRSPLICE(DTS_GPIO_OUT1, "gpio", STR_GPIO, "_out1");
		printk("[%s] [GPIO%s] DTS_GPIO_OUT0 = %s, DTS_GPIO_OUT1 = %s\n", __func__, STR_GPIO, DTS_GPIO_OUT0, DTS_GPIO_OUT1);
		printk("[%s] items_key = %s\n", __func__, items_key);

		if(!strcmp(TPD_RST, items_key)) {
			rst_output1 = pinctrl_lookup_state(pinctrl1, DTS_GPIO_OUT1);
			if (IS_ERR(rst_output1)) {
				ret = PTR_ERR(rst_output1);
				printk("%s : pinctrl err, rst_output1\n", __func__);
			}
			rst_output0 = pinctrl_lookup_state(pinctrl1, DTS_GPIO_OUT0);
			if (IS_ERR(rst_output0)) {
				ret = PTR_ERR(rst_output0);
				printk("%s : pinctrl err, rst_output0\n", __func__);
			}
		} else if(!strcmp(TPD_EINT, items_key)) {
			STRSPLICE(DTS_GPIO_EINT, "gpio", STR_GPIO, "_eint");
			eint_as_int = pinctrl_lookup_state(pinctrl1, DTS_GPIO_EINT);
			if (IS_ERR(eint_as_int)) {
				ret = PTR_ERR(eint_as_int);
				printk("%s : pinctrl err, eint_as_int\n", __func__);
				return ret;
			}
			eint_output1 = pinctrl_lookup_state(pinctrl1, DTS_GPIO_OUT1);
			if (IS_ERR(eint_output1)) {
				ret = PTR_ERR(eint_output1);
				printk("%s : pinctrl err, eint_output1\n", __func__);
			}
			eint_output0 = pinctrl_lookup_state(pinctrl1, DTS_GPIO_OUT0);
			if (IS_ERR(eint_output0)) {
				ret = PTR_ERR(eint_output0);
				printk("%s : pinctrl err, eint_output0\n", __func__);
			}
		} 
	} else {
		printk("[%s] \"%s\" not exist in items.ini !!! \n", __func__, items_key);
	} 

	return ret;
}


/*
 * items.ini file
 * for example:
 * touchpanel.power.num 	vgp1
 */
int _tp_pmic_config(char *items_key)
{
	char STR_PMIC[64];

	printk("[%s]\n", __func__);
	item_string(STR_PMIC, items_key, 0);
	printk("[%s] items_key = %s, STR_PMIC = %s .\n", __func__, items_key, STR_PMIC);

	if(!strcmp(TPD_PWR, items_key)) {
		tpd->reg = regulator_get(tpd->tpd_dev, STR_PMIC);
		if (IS_ERR(tpd->reg)) {
			printk("[%s] regulator_get() failed!\n", __func__);
		}
	} else {
		printk("[%s] regulator get cust tp node failed!\n", __func__);
		return -1;
	}

	return 0;
}

int TP_gsl_model_get(void)
{
	if(item_exist("touchpanel.gsl.modle"))
		item_string(name_tp_mode,"touchpanel.gsl.modle",0);
	else {
		printk("[%s] <touchpanel.gsl.modle> not exist in items.\n", __func__);
		return -1;
	}

	return 0;
}

int TP_gpio_config(void)
{
	int ret = 0;

	pinctrl1 = devm_pinctrl_get(tp_device);
	if (IS_ERR(pinctrl1)) {
		ret = PTR_ERR(pinctrl1);
		printk("[%s] fwq Cannot find touch pinctrl1!\n", __func__);
		return ret;
	}

	_tp_gpio_config("touchpanel.reset.pin");

	_tp_gpio_config("touchpanel.eint.pin");

	return 0;
}

int TP_i2c_config(void)
{
/*
	if(item_exist("touchpanel.i2c.num"))
		i2c_num = item_integer("touchpanel.i2c.num",0);
	else
		i2c_num = 0;
*/
	return 0;
}

int TP_pmic_config(void)
{
	if(item_exist("touchpanel.power.num")) {
		_tp_pmic_config("touchpanel.power.num");
	}
	return 0;
}

int TP_gsl_ic_config(void)
{


    if(item_equal("touchpanel.gsl.ic", "GSL1686", 0) || \
    	item_equal("touchpanel.gsl.ic", "GSL1686F", 0) || \
    	item_equal("touchpanel.gsl.ic", "GSL3675", 0) || \
    	item_equal("touchpanel.gsl.ic", "GSL3676", 0))
		touchpanel_gsl_ic = 0;	
    else if(item_equal("touchpanel.gsl.ic", "GSL915", 0) || \
    	item_equal("touchpanel.gsl.ic", "GSL3670", 0))
		touchpanel_gsl_ic = 1;


	return 0;
}

int TP_input_orientation_config(void)
{

	if(item_exist("ro.sf.hwrotation"))
		sf_hwrotation = item_integer("ro.sf.hwrotation",0);
	else
		sf_hwrotation = -1;

	if(item_exist("touchpanel.xy.swap"))
		xy_swap = item_integer("touchpanel.xy.swap",0);
	else
		xy_swap = -1;

	if(item_exist("touchpanel.x.reverse"))
		x_reverse = item_integer("touchpanel.x.reverse",0);
	else
		x_reverse = -1;

	if(item_exist("touchpanel.y.reverse"))
		y_reverse = item_integer("touchpanel.y.reverse",0);
	else
		y_reverse = -1;

	if(item_exist("touchpanel.xy.deal"))
		xy_deal = item_integer("touchpanel.xy.deal",0);
	else
		xy_deal = -1;

	if(item_exist("touchpanel.xy.deal.minor"))
		xy_deal_minor = item_integer("touchpanel.xy.deal.minor",0);
	else
		xy_deal_minor = -1;


	return 0;
}


int Elink_items_config_TP(void)
{

	printk("[%s] in.\n", __func__);

	TP_gsl_model_get();

	TP_gpio_config();

	TP_i2c_config();

	TP_pmic_config();

	TP_gsl_ic_config();

	TP_input_orientation_config();

	return 0;
}

int Elink_tp_device_get(struct platform_device *pdev)
{
	tp_device = &pdev->dev;
	return 0;
}
