/*
 * Copyright (c) 2016 Elinktek
 * Author: Yin gen
 * Email : yingen@elinktek.com
 */

#include <linux/init.h>		/* For init/exit macros */
#include <linux/module.h>	/* For MODULE_ marcros  */
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/platform_device.h>

#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/proc_fs.h>
#include <linux/rtc.h>
#include <linux/time.h>
#include <linux/slab.h>
#ifdef CONFIG_OF
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#endif

#include <asm/uaccess.h>

#include <mt-plat/mt_boot.h>
#include <mt-plat/mtk_rtc.h>

#include <mt-plat/mt_boot_reason.h>

#include <mt-plat/battery_meter.h>
#include <mt-plat/battery_common.h>
#include <mt-plat/battery_meter_hal.h>
#include <mach/mt_battery_meter.h>
//#include <mach/mt_battery_meter_table.h>
#include <mach/mt_pmic.h>
#include <mt-plat/upmu_common.h>

#include "elink_devconfig_bat.h"
#include <mach/elink_bat/elink_battery_list.h>
#include <mach/elink_bat/elink_mt_cust_default.h>
#include <mach/items.h>


char bat_info[64];
static unsigned int q_max_pos_50 = 0;
static unsigned int q_max_pos_25 = 0;
static unsigned int q_max_pos_0 = 0;
static unsigned int q_max_neg_10 = 0;
static unsigned int q_max_pos_50_h_current = 0;
static unsigned int q_max_pos_25_h_current = 0;
static unsigned int q_max_pos_0_h_current = 0;
static unsigned int q_max_neg_10_h_current = 0;
BATTERY_PROFILE_STRUCT_P t_battery_profile_t2 = NULL;
R_PROFILE_STRUCT_P t_r_profile_t2 = NULL;
struct battery_meter_custom_data *elink_batt_meter_cust_data = NULL;


extern BATTERY_PROFILE_STRUCT_P fgauge_get_profile(unsigned int temperature);
extern R_PROFILE_STRUCT_P fgauge_get_profile_r_table(unsigned int temperature);
extern int fgauge_get_saddles(void);
extern int fgauge_get_saddles_r_table(void);

int elink_fgauge_construct_battery_profile(BATTERY_PROFILE_STRUCT_P temp_profile_p)
{
#if defined(USE_NTC_BATTERY)
	return 1;
#else
	int i, saddles;

	BATTERY_PROFILE_STRUCT_P profile_p_25;
	
	profile_p_25 = fgauge_get_profile(TEMPERATURE_T2);
	
	saddles = fgauge_get_saddles();
	
	for (i = 0; i < saddles; i++)
    {
        (temp_profile_p + i)->voltage = (profile_p_25 + i)->voltage;
        (temp_profile_p + i)->percentage = (profile_p_25 + i)->percentage;
    }

    /* Dumpt new battery profile in MTKlog*/
	for (i = 0; i < saddles; i++) {
		bm_print(0, "<DOD,Voltage> at %d = <%d,%d>\r\n",
			 TEMPERATURE_T2, (temp_profile_p + i)->percentage,
			 (temp_profile_p + i)->voltage);
	}

    return 0;
	
#endif
}

int elink_fgauge_construct_r_table_profile(R_PROFILE_STRUCT_P temp_profile_p)
{
#if defined(USE_NTC_BATTERY)
	return 1;
#else
	int i, saddles;

	R_PROFILE_STRUCT_P profile_p_25;
	
	profile_p_25 = fgauge_get_profile_r_table(TEMPERATURE_T2);
	
	saddles = fgauge_get_saddles_r_table();
	
	for (i = 0; i < saddles; i++)
    {
        (temp_profile_p + i)->voltage = (profile_p_25 + i)->voltage;
        (temp_profile_p + i)->resistance = (profile_p_25 + i)->resistance;
    }

	/* Dumpt new r-table profile in MTKlog*/
	for (i = 0; i < saddles; i++) {
		bm_print(0, "<Rbat,VBAT> at %d = <%d,%d>\r\n",
			 TEMPERATURE_T2, (temp_profile_p + i)->resistance,
			 (temp_profile_p + i)->voltage);
	}

    return 0;
	
#endif
}

BATTERY_PROFILE_STRUCT_P __elink_fgauge_get_profile_in_items(void)
{
	printk("[%s] in.\n", __func__);

	return &t_battery_profile_t2[0];
}

R_PROFILE_STRUCT_P __elink_fgauge_get_profile_r_table_in_items(void)
{
	printk("[%s] in.\n", __func__);

	return &t_r_profile_t2[0];
}

int __elink_meter_init_cust_data_from_items(void)
{

	printk("[%s] in.\n", __func__);
	/* Qmax for battery  */
#if defined(USE_NTC_BATTERY)
	return 1;
#else 
	elink_batt_meter_cust_data->q_max_pos_50 = q_max_pos_50;

	elink_batt_meter_cust_data->q_max_pos_25 = q_max_pos_25;

	elink_batt_meter_cust_data->q_max_pos_0 = q_max_pos_0;

	elink_batt_meter_cust_data->q_max_neg_10 = q_max_neg_10;

	elink_batt_meter_cust_data->q_max_pos_50_h_current = q_max_pos_50_h_current;

	elink_batt_meter_cust_data->q_max_pos_25_h_current = q_max_pos_25_h_current;

	elink_batt_meter_cust_data->q_max_pos_0_h_current = q_max_pos_0_h_current;

	elink_batt_meter_cust_data->q_max_neg_10_h_current = q_max_neg_10_h_current;

	return 0;
#endif
}

int Elink_items_config_BAT(void)
{
	int i, batsizelist;
	int EK_Q_MAX_POS = 0;
	
	if(item_exist("battery.cust.elink")){
		item_string(bat_info,"battery.cust.elink",0);
		batsizelist = sizeof(battery_list) / sizeof(BATTERY_INFO_STRUCT);
		for (i = 0; i < batsizelist; i++){
			if( strcmp(bat_info, battery_list[i].name) == 0 ) break;
		}
		if(i == batsizelist){
			printk("\n[%s] <%s> from items isn't exist in list of battery.\n", __func__, bat_info);
			EK_Q_MAX_POS = Q_MAX_POS_25; 
			t_battery_profile_t2 = &elink_battery_profile_t2[0]; 
			t_r_profile_t2 = &elink_r_profile_t2[0];
		}else {
			printk("\n[%s] find <%s> in battery_list, battery capacity is %d.\n", __func__, battery_list[i].name, battery_list[i].s_q_max_pos_25);
			EK_Q_MAX_POS = battery_list[i].s_q_max_pos_25;
			t_battery_profile_t2 = battery_list[i].s_t_battery_profile_t2;
			t_r_profile_t2 = battery_list[i].s_t_r_profile_t2;
		}
	}else {
		EK_Q_MAX_POS = Q_MAX_POS_25; 
		t_battery_profile_t2 = &elink_battery_profile_t2[0]; 
		t_r_profile_t2 = &elink_r_profile_t2[0];
	}

	q_max_pos_50 = EK_Q_MAX_POS;
	q_max_pos_25 = EK_Q_MAX_POS;
	q_max_pos_0 = EK_Q_MAX_POS;
	q_max_neg_10 = EK_Q_MAX_POS;
	
	q_max_pos_50_h_current = EK_Q_MAX_POS;
	q_max_pos_25_h_current = EK_Q_MAX_POS;
	q_max_pos_0_h_current = EK_Q_MAX_POS;
	q_max_neg_10_h_current = EK_Q_MAX_POS;

	return 0;
	
}

int elink_get_batt_meter_cust_data(struct battery_meter_custom_data *p_batt_meter_cust_data)
{

	elink_batt_meter_cust_data = p_batt_meter_cust_data;

	return 0;
}
