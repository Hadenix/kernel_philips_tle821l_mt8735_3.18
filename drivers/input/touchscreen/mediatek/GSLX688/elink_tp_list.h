/******************************************************************************
 * elink_tp_list.h - Elink Android Linux Touch Panel Device Driver            *
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
#ifndef _ELINK_TP_LIST_H__
#define _ELINK_TP_LIST_H__

#include <linux/kernel.h>
#include "tpd_gsl_common.h"

//--------------------------------------------
//.h file
#include "TEST_COMPATIBILITY.h"
//590
#include "GSL1688_W590_DZ_OGS.h"
#include "GSL1688_W590_ZHZY_GG.h"
//601
//608
#include "GSL1688_W608_DZ_OGS.h"
#include "GSL1688_W608_QHD_DZ_OGS.h"
//698
#include "GSL1688_W698_DZ_OGS.h"
#include "GSL1688_W698_WSVGA_DZ_OGS.h"
//706
#include "GSL1688_W706_CTD_PG.h"
#include "GSL1688_W706_DZ_OGS.h"
#include "GSL1688_W706_FC_PG.h"
#include "GSL1688_W706_HT_PG.h"
#include "GSL1688_W706_SG_OGS.h"
#include "GSL1688_W706_MGL_PG.h"
#include "GSL1688_W706_XLL_PG.h"
#include "W706_GSL1688_CTD_PG_HAVE_KEY.h"
#include "W706_GSL1688_FC_PG_HAVE_KEY.h"
#include "W706_GSL1688_HT_PG_HAVE_KEY.h"
#include "GSL1688_W706_HK_PG_HAVE_KEY.h"
//718
#include "GSL1688_W718_FC_PG.h"
#include "GSL1688_W718_HT_PG.h"
#include "GSL1688_W718_OGS_DZ.h"
//785
#include "GSL3676_W785_PB_GG.h"
#include "GSL3676_W785_YTG_GG.h"
//798
#include "GSL3676_W798_DZH_GG.h"
#include "GSL3676_W798_DZ_OGS.h"
#include "GSL3676_W798_HH_OGS.h"
#include "GSL3676_W798_RSS_GG.h"
//801
#include "GSL3676_W801_CTD_PG.h"
#include "GSL3676_W801_DZH_GG.h"
#include "GSL3676_W801_DZ_OGS.h"
#include "GSL3676_W801_RSS_GG.h"
#include "GSL3676_W801_XCL_OGS.h"
#include "GSL3676_W801_ZC_GG.h"
//803
#include "GSL3670_W803_PB_GG.h"
//806
#include "GSL915_W806_DPT_PG.h"
#include "GSL915_W806_DZ_OGS.h"
#include "GSL915_W806_JND_OGS.h"
#include "GSL915_W806_WTGD_GG.h"
#include "GSL915_W806_YTG_GG.h"
//808
#include "GSL915_W808_HK_PG.h"
#include "GSL915_W808_FC_PG.h"

//101
#include "GSL3676_W101_RS_GG.h"
#include "GSL3676_W101_RSS_GG.h"
#include "GSL3676_W101_WTGD_GG.h"
#include "GSL3676_W101_YTG_GG.h"
#include "GSL3676_W101_WJ_PG.h"
#include "GSL3676_W101_FC_PG.h"
#include "GSL3676_W101_HT_PG.h"
#include "GSL3676_W101_ZHZY_PG.h"
//102
#include "GSL3676_W102_DH_PG.h"
#include "GSL3676_W102_ZHZY_PG.h"
#include "GSL3676_W102_YTG_GG.h"
#include "GSL3676_W102_FC_PG.h"
//103
#include "GSL3676_W103_HKGD_PG.h"
#include "GSL3676_W103_DH_PG.h"
#include "GSL3676_W103_WTGD_PG.h"
#include "GSL3676_W103_WJ_PG.h"
//108
#include "GSL3676_W108_YTG_GG.h"
//109
#include "GSL3676_W109_HT_PG.h"
//--------------------------------------------

//--------------------------------------------
//tp key
#ifdef TPD_HAVE_BUTTON 
static int tpd_keys_local[TPD_KEY_COUNT] = TPD_KEYS;
static int tpd_keys_local_home[1] = {KEY_HOMEPAGE};
//590
static int tpd_keys_dim_local_590_dz_ogs[TPD_KEY_COUNT][4] = TPD_KEYS_DIM_590_DZ_OGS;
static int tpd_keys_dim_local_590_zhzy_gg[TPD_KEY_COUNT][4] = TPD_KEYS_DIM_590_ZHZY_GG;
//706
static int tpd_keys_dim_local_706_fc_pg[TPD_KEY_COUNT][4] = TPD_KEYS_DIM_706_FC_PG;
static int tpd_keys_dim_local_706_ctd_pg[TPD_KEY_COUNT][4] = TPD_KEYS_DIM_706_CTD_PG; 
static int tpd_keys_dim_local_706_ht_pg[TPD_KEY_COUNT][4] = TPD_KEYS_DIM_706_HT_PG;
static int tpd_keys_dim_local_706_hk_pg[TPD_KEY_COUNT][4] = TPD_KEYS_DIM_706_HK_PG;  
//785
static int tpd_keys_dim_local_785_ytg_gg[TPD_KEY_COUNT][4] = TPD_KEYS_DIM_785_YTG_GG;
//798
static int tpd_keys_dim_local_798_dzh_gg[1][4] = TPD_KEYS_DIM_798_DZH_GG;
static int tpd_keys_dim_local_798_dz_ogs[1][4] = TPD_KEYS_DIM_798_DZ_OGS;
static int tpd_keys_dim_local_798_rss_gg[1][4] = TPD_KEYS_DIM_798_RSS_GG;
static int tpd_keys_dim_local_798_hh_ogs[1][4] = TPD_KEYS_DIM_798_HH_OGS;
//108
static int tpd_keys_dim_local_108_ytg_gg[TPD_KEY_COUNT][4] = TPD_KEYS_DIM_108_YTG_GG;
#endif
//--------------------------------------------

//--------------------------------------------
//tp_modle
typedef enum {
   //590
   mx590_tp_gsl_modle,
   //601
   //608
   mx608_tp_gsl_modle_dz_ogs,
   mx608_tp_gsl_modle_qhd_dz_ogs,
   //698
   mx698_tp_gsl_modle,
   mx698_tp_gsl_modle_wsvga,
   //706
   mx706_tp_gsl_modle,
   mx706_tp_gsl_modle_ctd_pg_have_key,
   mx706_tp_gsl_modle_fc_pg_have_key,
   mx706_tp_gsl_modle_ht_pg_have_key,
   mx706_tp_gsl_modle_hk_pg_have_key,
   //718
   mx718_tp_gsl_modle,
   //785
   mx785_tp_gsl_modle_ytg_gg,
   //798
   mx798_tp_gsl_modle,
   //801
   mx801_tp_gsl_modle,
   //803
   mx803_tp_gsl_modle,
   //806
   mx806_tp_gsl_modle,
   //808
   mx808_tp_gsl_modle,
   //101
   mx101_tp_gsl_modle,
   //102
   mx102_tp_gsl_modle,
   //103
   mx103_tp_gsl_modle,
   //108
   mx108_tp_gsl_modle_ytg_gg,
   //109
   mx109_tp_gsl_modle_ht_pg,

   //default
   default_tp_gsl_modle
}NUM_GSL_MODLE;
//--------------------------------------------

//--------------------------------------------

//590
TP_PROJECT_DATA mx590_tp_data[] = {
   {"MX590_DZ_OGS", {0xff,0x80,0x78,0x17}, gsl_config_data_id_590_DZ_OGS, GSL1688_FW_590_DZ_OGS, tpd_keys_dim_local_590_dz_ogs, ARRAY_SIZE(GSL1688_FW_590_DZ_OGS)},
   {"MX590_ZHZY_GG", {0xff,0x80,0x78,0x27}, gsl_config_data_id_590_ZHZY_GG, GSL1688_FW_590_ZHZY_GG, tpd_keys_dim_local_590_zhzy_gg, ARRAY_SIZE(GSL1688_FW_590_ZHZY_GG)},
   {"MX590_DEFAULT", {0xff,0xff,0xff,0xff}, gsl_config_data_id_590_DZ_OGS, GSL1688_FW_590_DZ_OGS, tpd_keys_dim_local_590_dz_ogs, ARRAY_SIZE(GSL1688_FW_590_DZ_OGS)},
};
//601
//608
TP_PROJECT_DATA mx608_tp_data_dz_ogs[] = {
   {"MX608_TP_DZ_OGS", {0xff,0xff,0xff,0xff}, gsl_config_data_id_608_DZ_OGS, GSL1688_FW_608_DZ_OGS, NULL, ARRAY_SIZE(GSL1688_FW_608_DZ_OGS)},
};
TP_PROJECT_DATA mx608_tp_data_qhd_dz_ogs[] = {
   {"MX608_TP_QHD_DZ_OGS", {0xff,0xff,0xff,0xff}, gsl_config_data_id_608_QHD_DZ_OGS, GSL1688_FW_608_QHD_DZ_OGS, NULL, ARRAY_SIZE(GSL1688_FW_608_QHD_DZ_OGS)},
};
//698
TP_PROJECT_DATA mx698_tp_data[] = {
   {"MX698_DZ_OGS", {0xfc,0x00,0x00,0x00}, gsl_config_data_id_698_DZ_OGS, GSL1688_FW_698_DZ_OGS, NULL, ARRAY_SIZE(GSL1688_FW_698_DZ_OGS)},
   {"MX698_DEFAULT", {0xff,0xff,0xff,0xff}, gsl_config_data_id_698_DZ_OGS, GSL1688_FW_698_DZ_OGS, NULL, ARRAY_SIZE(GSL1688_FW_698_DZ_OGS)},
};
TP_PROJECT_DATA mx698_tp_data_wsvga[] = {
   {"MX698_WSVGA_DZ_OGS", {0xfc,0x00,0x00,0x00}, gsl_config_data_id_698_WSVGA_DZ_OGS, GSL1688_FW_698_WSVGA_DZ_OGS, NULL, ARRAY_SIZE(GSL1688_FW_698_WSVGA_DZ_OGS)},
   {"MX698_WSVGA_DEFAULT", {0xff,0xff,0xff,0xff}, gsl_config_data_id_698_WSVGA_DZ_OGS, GSL1688_FW_698_WSVGA_DZ_OGS, NULL, ARRAY_SIZE(GSL1688_FW_698_WSVGA_DZ_OGS)},
};
//706
TP_PROJECT_DATA mx706_tp_data[] = {
   {"MX706_CTD_PG", {0xff,0x80,0x78,0x17}, gsl_config_data_id_706_CTD_PG, GSL1688_FW_706_CTD_PG, NULL, ARRAY_SIZE(GSL1688_FW_706_CTD_PG)},
   {"MX706_FC_PG", {0xff,0x80,0x78,0x0f}, gsl_config_data_id_706_FC_PG, GSL1688_FW_706_FC_PG, NULL, ARRAY_SIZE(GSL1688_FW_706_FC_PG)},
   {"MX706_DZ_OGS", {0xff,0x80,0x78,0x27}, gsl_config_data_id_706_DZ_OGS, GSL1688_FW_706_DZ_OGS, NULL, ARRAY_SIZE(GSL1688_FW_706_DZ_OGS)},
   {"MX706_SG_OGS", {0xff,0x80,0x78,0x47}, gsl_config_data_id_706_SG_OGS, GSL1688_FW_706_SG_OGS, NULL, ARRAY_SIZE(GSL1688_FW_706_SG_OGS)},
   {"MX706_HT_PG", {0xff,0xc0,0x78,0x07}, gsl_config_data_id_706_HT_PG, GSL1688_FW_706_HT_PG, NULL, ARRAY_SIZE(GSL1688_FW_706_HT_PG)},
   {"MX706_MGL_PG", {0xff,0x84,0x78,0x07}, gsl_config_data_id_706_MGL_PG, GSL1688_FW_706_MGL_PG, NULL, ARRAY_SIZE(GSL1688_FW_706_MGL_PG)},
   {"MX706_XLL_PG", {0xff,0x80,0x79,0x07}, gsl_config_data_id_706_XLL_PG, GSL1688_FW_706_XLL_PG, NULL, ARRAY_SIZE(GSL1688_FW_706_XLL_PG)},
   {"MX706_DEFAULT", {0xff,0xff,0xff,0xff}, gsl_config_data_id_706_CTD_PG, GSL1688_FW_706_CTD_PG, NULL, ARRAY_SIZE(GSL1688_FW_706_CTD_PG)},
};
TP_PROJECT_DATA mx706_tp_data_ctd_pg_have_key[] = {
   {"MX706_TP_CTD_PG_HAVE_KEY", {0xff,0xff,0xff,0xff}, gsl_config_data_id_706_CTD_PG_HAVE_KEY, GSL1688_FW_706_CTD_PG_HAVE_KEY, tpd_keys_dim_local_706_ctd_pg, ARRAY_SIZE(GSL1688_FW_706_CTD_PG_HAVE_KEY)},
};
TP_PROJECT_DATA mx706_tp_data_fc_pg_have_key[] = {
   {"MX706_TP_FC_PG_HAVE_KEY", {0xff,0xff,0xff,0xff}, gsl_config_data_id_706_FC_PG_HAVE_KEY, GSL1688_FW_706_FC_PG_HAVE_KEY, tpd_keys_dim_local_706_fc_pg, ARRAY_SIZE(GSL1688_FW_706_FC_PG_HAVE_KEY)},
};
TP_PROJECT_DATA mx706_tp_data_ht_pg_have_key[] = {
   {"MX706_TP_HT_PG_HAVE_KEY", {0xff,0xff,0xff,0xff}, gsl_config_data_id_706_HT_PG_HAVE_KEY, GSL1688_FW_706_HT_PG_HAVE_KEY, tpd_keys_dim_local_706_ht_pg, ARRAY_SIZE(GSL1688_FW_706_HT_PG_HAVE_KEY)},
};
TP_PROJECT_DATA mx706_tp_data_hk_pg_have_key[] = {
   {"MX706_TP_HK_PG_HAVE_KEY", {0xff,0xff,0xff,0xff}, gsl_config_data_id_W706_HK_PG_HAVE_KEY, GSL1688_FW_W706_HK_PG_HAVE_KEY, tpd_keys_dim_local_706_hk_pg, ARRAY_SIZE(GSL1688_FW_W706_HK_PG_HAVE_KEY)},
};
//718
TP_PROJECT_DATA mx718_tp_data[] = {
   {"MX718_DZ_OGS", {0xfc,0x00,0x00,0x00}, gsl_config_data_id_718_DZ_OGS, GSL1688_FW_718_DZ_OGS, NULL, ARRAY_SIZE(GSL1688_FW_718_DZ_OGS)},
   {"MX718_FC_PG", {0xfc,0x80,0x00,0x00}, gsl_config_data_id_718_FC_PG, GSL1688_FW_718_FC_PG, NULL, ARRAY_SIZE(GSL1688_FW_718_FC_PG)},
   {"MX718_HT_PG", {0xfc,0x10,0x00,0x00}, gsl_config_data_id_718_HT_PG, GSL1688_FW_718_HT_PG, NULL, ARRAY_SIZE(GSL1688_FW_718_HT_PG)},
   {"MX718_DEFAULT", {0xff,0xff,0xff,0xff}, gsl_config_data_id_718_DZ_OGS, GSL1688_FW_718_DZ_OGS, NULL, ARRAY_SIZE(GSL1688_FW_718_DZ_OGS)},
};
//785
TP_PROJECT_DATA mx785_tp_data_ytg_gg[] = {
   {"MX785_TP_YTG_GG", {0xff,0xff,0xff,0xff}, gsl_config_data_id_785_YTG_GG, GSL1688_FW_785_YTG_GG, tpd_keys_dim_local_785_ytg_gg, ARRAY_SIZE(GSL1688_FW_785_YTG_GG)},
};
//798
TP_PROJECT_DATA mx798_tp_data[] = {
   {"MX798_DZH_GG", {0x00,0x17,0x00,0x10}, gsl_config_data_id_798_DZH_GG, GSL3676_FW_798_DZH_GG, tpd_keys_dim_local_798_dzh_gg, ARRAY_SIZE(GSL3676_FW_798_DZH_GG)},
   {"MX798_DZ_OGS", {0x00,0x93,0x00,0x10}, gsl_config_data_id_798_DZ_OGS, GSL3676_FW_798_DZ_OGS, tpd_keys_dim_local_798_dz_ogs, ARRAY_SIZE(GSL3676_FW_798_DZ_OGS)},
   {"MX798_RSS_GG", {0x04,0x13,0x00,0x10}, gsl_config_data_id_798_RSS_GG, GSL3676_FW_798_RSS_GG, tpd_keys_dim_local_798_rss_gg, ARRAY_SIZE(GSL3676_FW_798_RSS_GG)},
   {"MX798_HH_OGS", {0x08,0x13,0x00,0x10}, gsl_config_data_id_798_HH_OGS, GSL3676_FW_798_HH_OGS, tpd_keys_dim_local_798_hh_ogs, ARRAY_SIZE(GSL3676_FW_798_HH_OGS)},
   {"MX798_DEFAULT", {0xff,0xff,0xff,0xff}, gsl_config_data_id_798_DZH_GG, GSL3676_FW_798_DZH_GG, tpd_keys_dim_local_798_dzh_gg, ARRAY_SIZE(GSL3676_FW_798_DZH_GG)},
};
//801
TP_PROJECT_DATA mx801_tp_data[] = {
   {"MX801_DZ_OGS", {0x00,0x93,0x00,0x10}, gsl_config_data_id_801_DZ_OGS, GSL3676_FW_801_DZ_OGS, NULL, ARRAY_SIZE(GSL3676_FW_801_DZ_OGS)},
   {"MX801_XCL_OGS", {0x00,0x33,0x00,0x10}, gsl_config_data_id_801_XCL_OGS, GSL3676_FW_801_XCL_OGS, NULL, ARRAY_SIZE(GSL3676_FW_801_XCL_OGS)},
   {"MX801_DZH_GG", {0x00,0x93,0x00,0x12}, gsl_config_data_id_801_DZH_GG, GSL3676_FW_801_DZH_GG, NULL, ARRAY_SIZE(GSL3676_FW_801_DZH_GG)},
   {"MX801_ZC_GG", {0x00,0x17,0x00,0x10}, gsl_config_data_id_801_ZC_GG, GSL3676_FW_801_ZC_GG, NULL, ARRAY_SIZE(GSL3676_FW_801_ZC_GG)},
   {"MX801_RSS_GG", {0x00,0x13,0x80,0x10}, gsl_config_data_id_801_RSS_GG, GSL3676_FW_801_RSS_GG, NULL, ARRAY_SIZE(GSL3676_FW_801_RSS_GG)},
   {"MX801_CTD_PG", {0x00,0x13,0xe0,0x10}, gsl_config_data_id_801_CTD_PG, GSL3676_FW_801_CTD_PG, NULL, ARRAY_SIZE(GSL3676_FW_801_CTD_PG)},
   {"MX801_DEFAULT", {0xff,0xff,0xff,0xff}, gsl_config_data_id_801_DZ_OGS, GSL3676_FW_801_DZ_OGS, NULL, ARRAY_SIZE(GSL3676_FW_801_DZ_OGS)},
};
//803
TP_PROJECT_DATA mx803_tp_data[] = {
   {"MX803_PB_COF", {0xff,0xff,0xff,0xff}, gsl_config_data_id_803_PB_GG, GSLX680_FW_803_PB_GG, NULL, ARRAY_SIZE(GSLX680_FW_803_PB_GG)},
   {"MX803_DEFAULT", {0xff,0xff,0xff,0xff}, gsl_config_data_id_803_PB_GG, GSLX680_FW_803_PB_GG, NULL, ARRAY_SIZE(GSLX680_FW_803_PB_GG)},
};
//806
TP_PROJECT_DATA mx806_tp_data[] = {
   {"MX806_WTGD_GG", {0xfc,0x00,0x08,0x00}, gsl_config_data_id_806_WTGD_GG, GSL915_FW_806_WTGD_GG, NULL, ARRAY_SIZE(GSL915_FW_806_WTGD_GG)},
   {"MX806_DZ_OGS", {0xfe,0x00,0x10,0x00}, gsl_config_data_id_806_DZ_OGS, GSL915_FW_806_DZ_OGS, NULL, ARRAY_SIZE(GSL915_FW_806_DZ_OGS)},
   {"MX806_JND_OGS", {0xfe,0x00,0x00,0x40}, gsl_config_data_id_806_JND_OGS, GSL915_FW_806_JND_OGS, NULL, ARRAY_SIZE(GSL915_FW_806_JND_OGS)},
   {"MX806_YTG_GG", {0xfc,0x00,0x00,0x80}, gsl_config_data_id_806_YTG_GG, GSL915_FW_806_YTG_GG, NULL, ARRAY_SIZE(GSL915_FW_806_YTG_GG)},
   {"MX806_DPT_PG", {0xfc,0x20,0x00,0x00}, gsl_config_data_id_806_DPT_PG, GSL915_FW_806_DPT_PG, NULL, ARRAY_SIZE(GSL915_FW_806_DPT_PG)},
   {"MX806_DEFAULT", {0xff,0xff,0xff,0xff}, gsl_config_data_id_806_WTGD_GG, GSL915_FW_806_WTGD_GG, NULL, ARRAY_SIZE(GSL915_FW_806_WTGD_GG)},
};
//808
TP_PROJECT_DATA mx808_tp_data[] = {
   {"MX808_HK_PG", {0xff,0xff,0xff,0xff}, gsl_config_data_id_808_HK_PG, GSLX680_FW_808_HK_PG, NULL, ARRAY_SIZE(GSLX680_FW_808_HK_PG)},  	
   {"MX808_FC_PG", {0xfe,0x04,0x00,0x00}, gsl_config_data_id_808_FC_PG, GSL915_FW_808_FC_PG, NULL, ARRAY_SIZE(GSL915_FW_808_FC_PG)},
};

//101
TP_PROJECT_DATA mx101_tp_data[] = {
   {"MX101_RSS_GG", {0x00,0x17,0x00,0x10}, gsl_config_data_id_101_RSS_GG, GSL3676_FW_101_RSS_GG, NULL, ARRAY_SIZE(GSL3676_FW_101_RSS_GG)},
//   {"MX101_WTGD_GG", {0x00,0x33,0x00,0x10}, gsl_config_data_id_101_WTGD_GG, GSL3676_FW_101_WTGD_GG, NULL, ARRAY_SIZE(GSL3676_FW_101_WTGD_GG)},
   {"MX101_WJ_PG", {0x01,0x13,0x00,0x10}, gsl_config_data_id_101_WJ_PG, GSL3676_FW_101_WJ_PG, NULL, ARRAY_SIZE(GSL3676_FW_101_WJ_PG)},
   {"MX101_RS_GG", {0x02,0x13,0x00,0x10}, gsl_config_data_id_101_RS_GG, GSL3676_FW_101_RS_GG, NULL, ARRAY_SIZE(GSL3676_FW_101_RS_GG)},
//   {"MX101_FC_PG", {0x00,0x33,0x00,0x10}, gsl_config_data_id_101_FC_PG, GSL3676_FW_101_FC_PG, NULL, ARRAY_SIZE(GSL3676_FW_101_FC_PG)},
//   {"MX101_HT_PG", {0x00,0x93,0x00,0x10}, gsl_config_data_id_101_HT_PG, GSL3676_FW_101_HT_PG, NULL, ARRAY_SIZE(GSL3676_FW_101_HT_PG)},
   {"MX101_DH_PG", {0x00,0x53,0x00,0x10}, gsl_config_data_id_102_DH_PG, GSL3676_FW_102_DH_PG, NULL, ARRAY_SIZE(GSL3676_FW_102_DH_PG)},
   {"MX101_ZHZY_PG", {0x00,0x33,0x00,0x10}, gsl_config_data_id_101_ZHZY_GG, GSL3676_FW_101_ZHZY_GG, NULL, ARRAY_SIZE(GSL3676_FW_101_ZHZY_GG)},
   {"MX101_YTG_PG", {0x00,0x93,0x00,0x10}, gsl_config_data_id_101_YTG_GG, GSL3676_FW_101_YTG_GG, NULL, ARRAY_SIZE(GSL3676_FW_101_YTG_GG)},
   {"MX101_DEFAULT", {0xff,0xff,0xff,0xff}, gsl_config_data_id_101_RSS_GG, GSL3676_FW_101_RSS_GG, NULL, ARRAY_SIZE(GSL3676_FW_101_RSS_GG)},
};
TP_PROJECT_DATA mx101_tp_data_temp[] = {
   {"MX101_FC_PG_TEMP", {0x00,0x33,0x00,0x10}, gsl_config_data_id_101_FC_PG, GSL3676_FW_101_FC_PG, NULL, ARRAY_SIZE(GSL3676_FW_101_FC_PG)},
   {"MX101_HT_PG_TEMP", {0x00,0x93,0x00,0x10}, gsl_config_data_id_101_HT_PG, GSL3676_FW_101_HT_PG, NULL, ARRAY_SIZE(GSL3676_FW_101_HT_PG)},
   {"MX101_DEFAULT_TEMP", {0xff,0xff,0xff,0xff}, gsl_config_data_id_101_FC_PG, GSL3676_FW_101_FC_PG, NULL, ARRAY_SIZE(GSL3676_FW_101_FC_PG)},

};
//102
TP_PROJECT_DATA mx102_tp_data[] = {
   {"MX102_DH_PG", {0x00,0x53,0x00,0x10}, gsl_config_data_id_102_DH_PG, GSL3676_FW_102_DH_PG, NULL, ARRAY_SIZE(GSL3676_FW_102_DH_PG)},
   {"MX102_ZHZY_PG", {0x00,0x13,0x00,0x10}, gsl_config_data_id_102_ZHZY_PG, GSL3676_FW_102_ZHZY_PG, NULL, ARRAY_SIZE(GSL3676_FW_102_ZHZY_PG)},
   {"MX102_YTG_GG", {0x00,0x93,0x00,0x10}, gsl_config_data_id_102_YTG_GG, GSL3676_FW_102_YTG_GG, NULL, ARRAY_SIZE(GSL3676_FW_102_YTG_GG)},
//   {"MX102_FC_PG", {0x00,0x13,0x00,0x10}, gsl_config_data_id_102_FC_PG, GSL3676_FW_102_FC_PG, NULL, ARRAY_SIZE(GSL3676_FW_102_FC_PG)},
   {"MX102_DEFAULT", {0xff,0xff,0xff,0xff}, gsl_config_data_id_102_ZHZY_PG, GSL3676_FW_102_ZHZY_PG, NULL, ARRAY_SIZE(GSL3676_FW_102_ZHZY_PG)},
};

//103
TP_PROJECT_DATA mx103_tp_data[] = {
   {"MX103_HKGD_PG", {0x00,0x17,0x00,0x10}, gsl_config_data_id_103_HKGD_PG, GSL3676_FW_103_HKGD_PG, NULL, ARRAY_SIZE(GSL3676_FW_103_HKGD_PG)},
   {"MX103_DH_PG", {0x00,0x33,0x00,0x10}, gsl_config_data_id_103_DH_PG, GSL3676_FW_103_DH_PG, NULL, ARRAY_SIZE(GSL3676_FW_103_DH_PG)},
   {"MX103_WTGD_PG", {0x00,0x1b,0x00,0x10}, gsl_config_data_id_103_WTGD_PG, GSL3676_FW_103_WTGD_PG, NULL, ARRAY_SIZE(GSL3676_FW_103_WTGD_PG)},
   {"MX103_WJ_PG", {0x00,0x13,0x20,0x10}, gsl_config_data_id_103_WJ_PG, GSL3676_FW_103_WJ_PG, NULL, ARRAY_SIZE(GSL3676_FW_103_WJ_PG)},
   {"MX103_DEFAULT", {0xff,0xff,0xff,0xff}, gsl_config_data_id_103_HKGD_PG, GSL3676_FW_103_HKGD_PG, NULL, ARRAY_SIZE(GSL3676_FW_103_HKGD_PG)},
};
//108
TP_PROJECT_DATA mx108_tp_data_gg_ytg[] = {
   {"MX108_TP_YTG_GG", {0xff,0xff,0xff,0xff}, gsl_config_data_id_108_YTG_GG, GSL3676_FW_108_YTG_GG, tpd_keys_dim_local_108_ytg_gg, ARRAY_SIZE(GSL3676_FW_108_YTG_GG)},
};
//109
TP_PROJECT_DATA mx109_tp_data_ht_pg[] = {
   {"MX109_TP_HT_PG", {0xff,0xff,0xff,0xff}, gsl_config_data_id_109_HT_PG, GSL3676_FW_109_HT_PG, NULL, ARRAY_SIZE(GSL3676_FW_109_HT_PG)},
};
//--------------------------------------------

//--------------------------------------------

ALL_TP_INFO gsl_tp_info[] = {
   //590
   {"MX590_TP", mx590_tp_gsl_modle, mx590_tp_data, 3, ARRAY_SIZE(mx590_tp_data), 0},
   //601
   //608
   {"MX608_TP_DZ_OGS", mx608_tp_gsl_modle_dz_ogs, mx608_tp_data_dz_ogs, 0, ARRAY_SIZE(mx608_tp_data_dz_ogs), 0},   
   {"MX608_TP_QHD_DZ_OGS", mx608_tp_gsl_modle_qhd_dz_ogs, mx608_tp_data_qhd_dz_ogs, 0, ARRAY_SIZE(mx608_tp_data_qhd_dz_ogs), 0},
   //698
   {"MX698_TP", mx698_tp_gsl_modle, mx698_tp_data, 0, ARRAY_SIZE(mx698_tp_data), 0},   
   {"MX698_TP_WSVGA", mx698_tp_gsl_modle_wsvga, mx698_tp_data_wsvga, 0, ARRAY_SIZE(mx698_tp_data_wsvga), 0},   
   //706
   {"MX706_TP", mx706_tp_gsl_modle, mx706_tp_data, 0, ARRAY_SIZE(mx706_tp_data), 0},
   {"MX706_TP_CTD_PG_HAVE_KEY", mx706_tp_gsl_modle_ctd_pg_have_key, mx706_tp_data_ctd_pg_have_key, 3, ARRAY_SIZE(mx706_tp_data_ctd_pg_have_key), 0},
   {"MX706_TP_FC_PG_HAVE_KEY", mx706_tp_gsl_modle_fc_pg_have_key, mx706_tp_data_fc_pg_have_key, 3, ARRAY_SIZE(mx706_tp_data_fc_pg_have_key), 0},
   {"MX706_TP_HT_PG_HAVE_KEY", mx706_tp_gsl_modle_ht_pg_have_key, mx706_tp_data_ht_pg_have_key, 3, ARRAY_SIZE(mx706_tp_data_ht_pg_have_key), 0},
   {"MX706_TP_HK_PG_HAVE_KEY", mx706_tp_gsl_modle_hk_pg_have_key, mx706_tp_data_hk_pg_have_key, 3, ARRAY_SIZE(mx706_tp_data_hk_pg_have_key), 0},
   //718
   {"MX718_TP", mx718_tp_gsl_modle, mx718_tp_data, 0, ARRAY_SIZE(mx718_tp_data), 0},
   //785
   {"MX785_TP_YTG_GG", mx785_tp_gsl_modle_ytg_gg, mx785_tp_data_ytg_gg, 3, ARRAY_SIZE(mx785_tp_data_ytg_gg), 19},
   //798
   {"MX798_TP", mx798_tp_gsl_modle, mx798_tp_data, 1, ARRAY_SIZE(mx798_tp_data), 19},
   //801
   {"MX801_TP", mx801_tp_gsl_modle, mx801_tp_data, 0, ARRAY_SIZE(mx801_tp_data), 0},
   //803
   {"MX803_TP", mx803_tp_gsl_modle, mx803_tp_data, 0, ARRAY_SIZE(mx803_tp_data), 0},
   //806
   {"MX806_TP", mx806_tp_gsl_modle, mx806_tp_data, 0, ARRAY_SIZE(mx806_tp_data), 0},
   //808
   {"MX808_TP", mx808_tp_gsl_modle, mx808_tp_data, 0, ARRAY_SIZE(mx808_tp_data), 0},
   //101
   {"MX101_TP", mx101_tp_gsl_modle, mx101_tp_data, 0, ARRAY_SIZE(mx101_tp_data), 0},
   {"MX101_TP_TEMP", mx101_tp_gsl_modle, mx101_tp_data_temp, 0, ARRAY_SIZE(mx101_tp_data_temp), 0},
   //102
   {"MX102_TP", mx102_tp_gsl_modle, mx102_tp_data, 0, ARRAY_SIZE(mx102_tp_data), 0},
   //103
   {"MX103_TP", mx103_tp_gsl_modle, mx103_tp_data, 0, ARRAY_SIZE(mx103_tp_data), 0},
   //108
   {"MX108_TP_YTG_GG", mx108_tp_gsl_modle_ytg_gg, mx108_tp_data_gg_ytg, 3, ARRAY_SIZE(mx108_tp_data_gg_ytg), 61},
   //109
   {"MX109_TP_HT_PG", mx109_tp_gsl_modle_ht_pg, mx109_tp_data_ht_pg, 0, ARRAY_SIZE(mx109_tp_data_ht_pg), 0},

};



#endif
