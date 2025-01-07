/*
 * Copyright (c) 2016 Elinktek
 * Author: Yin gen
 * Email : yingen@elinktek.com
 */
#ifndef _ELINK_DEVCONFIG_BAT_H_
#define _ELINK_DEVCONFIG_BAT_H_

#include <mach/elink_bat/elink_battery_meter.h>

int elink_fgauge_construct_battery_profile(BATTERY_PROFILE_STRUCT_P temp_profile_p);
int elink_fgauge_construct_r_table_profile(R_PROFILE_STRUCT_P temp_profile_p);
int elink_get_batt_meter_cust_data(struct battery_meter_custom_data *p_batt_meter_cust_data);
int Elink_items_config_BAT(void);
int __elink_meter_init_cust_data_from_items(void);
R_PROFILE_STRUCT_P __elink_fgauge_get_profile_r_table_in_items(void);
BATTERY_PROFILE_STRUCT_P __elink_fgauge_get_profile_in_items(void);

#endif
