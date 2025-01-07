/*
 * Copyright (c) 2016 Elinktek
 * Author: Yin gen
 * Email : yingen@elinktek.com
 */
#ifndef _ELINK_DEVCONFIG_CAM_H_
#define _ELINK_DEVCONFIG_CAM_H_

#include <linux/types.h>
#include "kd_camera_typedef.h"
#include "kd_camera_hw.h"

#if defined CONFIG_MTK_LEGACY
#include <mach/mt_gpio.h>
#include <mach/mt_pm_ldo.h>
#include "pmic_drv.h"
#endif /* End of #if defined CONFIG_MTK_LEGACY */

/******************************************************************************
 * Debug configuration
******************************************************************************/
#define IMG_NAME_LEN 32

#define VOL_2800 2800000
#define VOL_1800 1800000
#define VOL_1500 1500000
#define VOL_1200 1200000
#define VOL_1000 1000000

#define IDX_MAIN 0
#define IDX_SUB 1

#define IDX_PS_AVDD 0
#define IDX_PS_DVDD 1
#define IDX_PS_IOVDD 2
#define IDX_PS_AFVDD 3

#define IDX_PS_CMRST 0
#define IDX_PS_CMPDN 4
#define IDX_PS_MODE 1
#define IDX_PS_ON   2
#define IDX_PS_OFF  3

#define CMRST0 "camera.main.rst.pin"
#define CMPDN0 "camera.main.pwd.pin"
#define CMRST1 "camera.sub.rst.pin"
#define CMPDN1 "camera.sub.pwd.pin"

#define MAIN_AVDD "camera.main.power.avdd"
#define SUB_AVDD "camera.sub.power.avdd"
#define MAIN_DVDD "camera.main.power.dvdd"
#define SUB_DVDD "camera.sub.power.dvdd"
#define MAIN_IOVDD "camera.main.power.iovdd"
#define SUB_IOVDD "camera.sub.power.iovdd"
#define MAIN_AFVDD "camera.main.power.afvdd"
#define SUB_AFVDD "camera.sub.power.afvdd"


typedef struct img_info {
	char name[IMG_NAME_LEN];
	u32 vcam_a_volt;	// avdd
	u32 vcam_d_volt;	// dvdd
	u32 vcam_d2_volt;	// iovdd
	u32 vcam_a2_volt;	// afvdd
	u32 rst_on;			// sensor work
	u32 rst_off;		// sensor reset
	u32 pdn_on;			// sensor work
	u32 pdn_off;		// sensor sleep
}IMG_INFO_STRUCT;


int Interface_check(char *currSensorName, int pinSetIdx, int On, char* mode_name);
bool _hwPowerOnCnt(KD_REGULATOR_TYPE_T powerId, int powerVolt, char *mode_name);
bool _hwPowerDownCnt(KD_REGULATOR_TYPE_T powerId, char *mode_name);
IMG_INFO_STRUCT* Camera_list_config(u32 (*pinSet)[8], u32 (*pmicSet)[4], char *currSensorName);
int Elink_items_config_CAM(void);
int Elink_cam_device_get(struct platform_device *pdev);
int Elink_cam_pmic_regulator_get(u32 (*pmic_reg)[4]);

extern struct regulator *regVCAMA;
extern struct regulator *regVCAMD;
extern struct regulator *regVCAMIO;
extern struct regulator *regVCAMAF;
extern struct regulator *regSubVCAMD;
extern struct platform_device *cam_plt_dev;
extern struct pinctrl *camctrl;
extern struct pinctrl_state *cam0_pnd_h;
extern struct pinctrl_state *cam0_pnd_l;
extern struct pinctrl_state *cam0_rst_h;
extern struct pinctrl_state *cam0_rst_l;
extern struct pinctrl_state *cam1_pnd_h;
extern struct pinctrl_state *cam1_pnd_l;
extern struct pinctrl_state *cam1_rst_h;
extern struct pinctrl_state *cam1_rst_l;
extern struct pinctrl_state *cam_ldo0_h;
extern struct pinctrl_state *cam_ldo0_l;
extern struct device *sensor_device;

#endif
