/******************************************************************************
 * elink_devconfig_tp.h - Elink Android Linux Touch Panel Device Driver       *
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
#ifndef _ELINK_DEVCONFIG_TP_H__
#define _ELINK_DEVCONFIG_TP_H__

#include <linux/kernel.h>
#include "tpd_gsl_common.h"

#define TP_NAME_LEN 32

typedef struct tp_project_data {
   char name[TP_NAME_LEN];
   u8 tp_id[4];
   unsigned int *p_gsl_config_data_id;
   const struct fw_data *p_fw;
   int (*tpd_keys)[4];
   unsigned int tp_source_len;
}TP_PROJECT_DATA;


typedef struct all_tp_info {
   char name[TP_NAME_LEN];
   unsigned int tp_gsl_modle;
   TP_PROJECT_DATA *pro_tp_data;
   int HAVE_KEY;
   unsigned int tp_num;
   int gpio_led_pin;
}ALL_TP_INFO;


extern unsigned int *PTR_GSL_CONFIG_DATA_ID;
extern const struct fw_data *PTR_FW;
extern char name_tp_mode[64];
extern char *NAME_TP;
extern unsigned int TP_SOURCE_LEN;

extern int KEY_NUM;
extern int TP_NUM;
extern int LED_PIN;
extern int (*PTR_TPD_KEYS_DIM_LOCAL)[4];

extern TP_PROJECT_DATA *PTR_TP_PROJECT_DATA;

extern int touchpanel_gsl_ic;

extern struct tpd_device *tpd;

extern struct pinctrl *pinctrl1;
extern struct pinctrl_state *eint_as_int;
extern struct pinctrl_state *eint_output0;
extern struct pinctrl_state *eint_output1;
extern struct pinctrl_state *rst_output0;
extern struct pinctrl_state *rst_output1;

#define TPD_RST "touchpanel.reset.pin"
#define TPD_EINT "touchpanel.eint.pin"
#define TPD_PWR "touchpanel.power.num"

#define VOL_2800 2800000
#define VOL_1800 1800000
#define VOL_1500 1500000
#define VOL_1200 1200000
#define VOL_1000 1000000

int tp_list_config(void);
int match_gsl_tp_id(char *read_buf);
int control_key_light(int x, int y);
int match_gsl_tp_button(void);
int key_light_init_timer(void);
int key_light_mod_timer(void);
int Elink_tp_device_get(struct platform_device *pdev);
int control_input_orientation(int *x, int *y, int screenwidth, int screenheight);
int tpd_hwPowerControl(unsigned int powerVolt, char *name, bool on);
int Elink_items_config_TP(void);

#endif