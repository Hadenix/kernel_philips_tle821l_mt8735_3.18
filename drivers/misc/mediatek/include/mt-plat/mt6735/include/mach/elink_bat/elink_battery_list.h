/*
 * Copyright (c) 2016 Elinktek
 * Author: Yin gen
 * Email : yingen@elinktek.com
 */
#ifndef _ELINK_BATTERY_LIST_H_
#define _ELINK_BATTERY_LIST_H_

//For 601
#include "BM_601_JFY_2600.h"
#include "BM_601_JJY_2600.h"
#include "BM_606_JFY_2500.h"
#include "BM_606_JJY_2500.h"
//For 650
#include "BM_650_AB_2200.h"
#include "BM_650_NV_2200.h"
//For 680
#include "BM_680_AB_2700.h"
//For 706
#include "BM_706_AB_2800.h"
#include "BM_706_CTE_2800.h"
#include "BM_706_NV_2800.h"
//For 801
#include "BM_801_CTE_3800.h"
//For 790
#include "BM_790_JFY_4000.h"
#include "BM_790_SJY_4000.h"
//For 792
#include "BM_792_CTE_3000.h"
//For 798
#include "BM_798_CTE_3000.h"
#include "BM_798_PT_3000.h"
//For 900
#include "BM_900_JFY_4500.h"
#include "BM_900_JZ_4500.h"
//For 901
#include "BM_901_TRS_5500.h"
#include "BM_901_YD_5800.h"
//For 960
#include "BM_960_TRS_4700.h"
#include "BM_960_YD_5000.h"
//For 970
#include "BM_970_JFY_7000.h"
//For 978
#include "BM_978_YDNY_5800.h"
//For 100
#include "BM_100_JFY_5200.h"
#include "BM_100_JFY_5600.h"
#include "BM_100_JZ_6000.h"
//For 101
#include "BM_101_YD_6300.h"
//For 102
#include "BM_102_YD_6300.h"
//For 103
#include "BM_103_YD_6000.h"


BATTERY_INFO_STRUCT battery_list[] = {
	//For 601
	{"BM_601_JFY_2600", BM_601_JFY_2600_Q_MAX_POS_25, bm_601_jfy_2600_battery_profile_t2, bm_601_jfy_2600_r_profile_t2},
	{"BM_601_JJY_2600", BM_601_JJY_2600_Q_MAX_POS_25, bm_601_jjy_2600_battery_profile_t2, bm_601_jjy_2600_r_profile_t2},
	{"BM_606_JFY_2500", BM_606_JFY_2500_Q_MAX_POS_25, bm_606_jfy_2500_battery_profile_t2, bm_606_jfy_2500_r_profile_t2},
	{"BM_606_JJY_2500", BM_606_JJY_2500_Q_MAX_POS_25, bm_606_jjy_2500_battery_profile_t2, bm_606_jjy_2500_r_profile_t2},
	//For 650
	{"BM_650_AB_2200", BM_650_AB_2200_Q_MAX_POS_25, bm_650_ab_2200_battery_profile_t2, bm_650_ab_2200_r_profile_t2},
	{"BM_650_NV_2200", BM_650_NV_2200_Q_MAX_POS_25, bm_650_nv_2200_battery_profile_t2, bm_650_nv_2200_r_profile_t2},
	//For 680
	{"BM_680_AB_2700", BM_680_AB_2700_Q_MAX_POS_25, bm_680_ab_2700_battery_profile_t2, bm_680_ab_2700_r_profile_t2},
	//For 706
	{"BM_706_AB_2800", BM_706_AB_2800_Q_MAX_POS_25, bm_706_ab_2800_battery_profile_t2, bm_706_ab_2800_r_profile_t2},
	{"BM_706_CTE_2800", BM_706_CTE_2800_Q_MAX_POS_25, bm_706_cte_2800_battery_profile_t2, bm_706_cte_2800_r_profile_t2},
	{"BM_706_NV_2800", BM_706_NV_2800_Q_MAX_POS_25, bm_706_nv_2800_battery_profile_t2, bm_706_nv_2800_r_profile_t2},
	//For 801
	{"BM_801_CTE_3800", BM_801_CTE_3800_Q_MAX_POS_25, bm_801_cte_3800_battery_profile_t2, bm_801_cte_3800_r_profile_t2},
	//For 790
	{"BM_790_JFY_4000", BM_790_JFY_4000_Q_MAX_POS_25, bm_790_jfy_4000_battery_profile_t2, bm_790_jfy_4000_r_profile_t2},
	{"BM_790_SJY_4000", BM_790_SJY_4000_Q_MAX_POS_25, bm_790_sjy_4000_battery_profile_t2, bm_790_sjy_4000_r_profile_t2},
	//For 792
	{"BM_792_CTE_3000", BM_790_SJY_4000_Q_MAX_POS_25, bm_790_sjy_4000_battery_profile_t2, bm_790_sjy_4000_r_profile_t2},
	//For 798
	{"BM_798_CTE_3000", BM_798_CTE_3000_Q_MAX_POS_25, bm_798_cte_3000_battery_profile_t2, bm_798_cte_3000_r_profile_t2},
	{"BM_798_PT_3000", BM_798_PT_3000_Q_MAX_POS_25, bm_798_pt_3000_battery_profile_t2, bm_798_pt_3000_r_profile_t2},
	//For 900
	{"BM_900_JFY_4500", BM_900_JFY_4500_Q_MAX_POS_25, bm_900_jfy_4500_battery_profile_t2, bm_900_jfy_4500_r_profile_t2},
	{"BM_900_JZ_4500", BM_900_JZ_4500_Q_MAX_POS_25, bm_900_jz_4500_battery_profile_t2, bm_900_jz_4500_r_profile_t2},
	//For 901
	{"BM_901_TRS_5500", BM_901_TRS_5500_Q_MAX_POS_25, bm_901_trs_5500_battery_profile_t2, bm_901_trs_5500_r_profile_t2},
	{"BM_901_YD_5800", BM_901_YD_5800_Q_MAX_POS_25, bm_901_yd_5800_battery_profile_t2, bm_901_yd_5800_r_profile_t2},
	//For 960
	{"BM_960_TRS_4700", BM_960_TRS_4700_Q_MAX_POS_25, bm_960_trs_4700_battery_profile_t2, bm_960_trs_4700_r_profile_t2},
	{"BM_960_YD_5000", BM_960_YD_5000_Q_MAX_POS_25, bm_960_yd_5000_battery_profile_t2, bm_960_yd_5000_r_profile_t2},
	//For 970
	{"BM_970_JFY_7000", BM_970_JFY_7000_Q_MAX_POS_25, bm_970_jfy_7000_battery_profile_t2, bm_970_jfy_7000_r_profile_t2},
	//For 978
	{"BM_978_YDNY_5800", BM_978_YDNY_5800_Q_MAX_POS_25, bm_978_ydny_5800_battery_profile_t2, bm_978_ydny_5800_r_profile_t2},
	//For 100
	{"BM_100_JFY_5200", BM_100_JFY_5200_Q_MAX_POS_25, bm_100_jfy_5200_battery_profile_t2, bm_100_jfy_5200_r_profile_t2},
	{"BM_100_JFY_5600", BM_100_JFY_5600_Q_MAX_POS_25, bm_100_jfy_5600_battery_profile_t2, bm_100_jfy_5600_r_profile_t2},
	{"BM_100_JZ_6000", BM_100_JZ_6000_Q_MAX_POS_25, bm_100_jz_6000_battery_profile_t2, bm_100_jz_6000_r_profile_t2},
	//For 101
	{"BM_101_YD_6300", BM_101_YD_6300_Q_MAX_POS_25, bm_101_yd_6300_battery_profile_t2, bm_101_yd_6300_r_profile_t2},
	//For 102
	{"BM_102_YD_6300", BM_102_YD_6300_Q_MAX_POS_25, bm_102_yd_6300_battery_profile_t2, bm_102_yd_6300_r_profile_t2},
	//For 103
	{"BM_103_YD_6000", BM_103_YD_6000_Q_MAX_POS_25, bm_103_yd_6000_battery_profile_t2, bm_103_yd_6000_r_profile_t2},
};

#endif //#ifndef _ELINK_BATTERY_LIST_H_
