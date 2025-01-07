/*
 * Copyright (c) 2016 Elinktek
 * Author: Yin gen
 * Email : yingen@elinktek.com
 */

#ifndef _ELINK_BATTERY_METER_H
#define _ELINK_BATTERY_METER_H

typedef struct _BATTERY_PROFILE_STRUCT {
	signed int percentage;
	signed int voltage;
} BATTERY_PROFILE_STRUCT, *BATTERY_PROFILE_STRUCT_P;

typedef struct _R_PROFILE_STRUCT {
	signed int resistance; /* Ohm*/
	signed int voltage;
} R_PROFILE_STRUCT, *R_PROFILE_STRUCT_P;

#define BAT_NAME_LEN 32
typedef struct battery_info {
	char name[BAT_NAME_LEN];
	unsigned int  s_q_max_pos_25;
	BATTERY_PROFILE_STRUCT *s_t_battery_profile_t2;
	R_PROFILE_STRUCT *s_t_r_profile_t2;
}BATTERY_INFO_STRUCT;

#endif	//#ifndef _ELINK_BATTERY_METER_H