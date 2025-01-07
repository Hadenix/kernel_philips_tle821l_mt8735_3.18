/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
#include <linux/string.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/pinctrl/consumer.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <asm-generic/gpio.h>

//#include <mach/mt_gpio.h>
//#include <mach/mt_pm_ldo.h>
//#include <mach/upmu_common.h>
	
#include "lcm_drv.h"
#include "ddp_irq.h"

#include <mach/items.h>
#include "elink_devconfig_lcm.h"
static unsigned int GPIO_LCD_PWR_EN;
//static unsigned int GPIO_LCD_BL_EN;
static unsigned int GPIO_LCD_TE_EN;
static unsigned int GPIO_LCD_RST_EN;

//static struct regulator *lcm_vgp;

#define init_lcm_gpio(a, b) init_lcm_gpio(#a, b)

/**
 * Local Constants
 */
#define FRAME_WIDTH		(800)
#define FRAME_HEIGHT		(1280)

#define REGFLAG_DELAY		0xFE
#define REGFLAG_END_OF_TABLE	0xFF   /* END OF REGISTERS MARKER */

#define LCM_DSI_CMD_MODE	0

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))
#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)										lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   			lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)    
#ifndef ASSERT
#define ASSERT(expr)					\
	do {						\
		if (expr)				\
			break;				\
		pr_debug("DDP ASSERT FAILED %s, %d\n",	\
		       __FILE__, __LINE__);		\
		BUG();					\
	} while (0)
#endif


// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------
struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};

/**
 * Note :
 *
 * Data ID will depends on the following rule.
 *
 * count of parameters > 1	=> Data ID = 0x39
 * count of parameters = 1	=> Data ID = 0x15
 * count of parameters = 0	=> Data ID = 0x05
 *
 * Structure Format :
 *
 * {DCS command, count of parameters, {parameter list}}
 * {REGFLAG_DELAY, milliseconds of time, {} },
 * ...
 *
 * Setting ending by predefined flag
 *
 * {REGFLAG_END_OF_TABLE, 0x00, {}}
 */
#if 1
static struct LCM_setting_table lcm_initialization_setting[] = {
	{0xFF,4,{0xAA,0x55,0xA5,0x80}},//========== Internal setting ==========
{0x6F,2,{0x11,0x00}},
{0xF7,2,{0x20,0x00}},
{0x6F,1,{0x06}},
{0xF7,1,{0xA0}},
{0x6F,1,{0x19}},
{0xF7,1,{0x12}},
{0xF4,1,{0x03}},
// new Vcom floating
{0x6F,1,{0x08}},
{0xFA,1,{0x40}},
{0x6F,1,{0x11}},
{0xF3,1,{0x01}},
// for1, abnormal power off
{0x6F,1,{0x06}},
{0xFC,1,{0x03}},
{0xF0,5,{0x55,0xAA,0x52,0x08,0x00}},//========== page0 relative ==========
{0xB1,2,{0x68,0x01}},
{0xB6,1,{0x08}},
{0x6F,1,{0x02}},
{0xB8,1,{0x08}},
{0xBB,2,{0x54,0x44}},
{0xBC,2,{0x05,0x05}},
{0xC7,1,{0x01}},
{0xBD,5,{0x02,0xB0,0x1E,0x1E,0x00}},
{0xC5,2,{0x01,0x07}},
{0xC8,1,{0x80}},
{0xF0,5,{0x55,0xAA,0x52,0x08,0x01}},//========== page1 relative ==========
{0xB0,2,{0x05,0x05}},
{0xB1,2,{0x05,0x05}},
//new}1,
{0xB2,2,{0x00,0x00}},
//{0xB8,2,{0x03,0x03}},
{0xBC,2,{0x90,0x01}},
{0xBD,2,{0x90,0x01}},
{0xCA,1,{0x00}},
{0xC0,1,{0x04}},
{0xBE,1,{0x29}},
{0xB3,2,{0x28,0x28}},
{0xB4,2,{0x12,0x12}},
{0xB9,2,{0x35,0x35}},
{0xBA,2,{0x25,0x25}},
{0xF0,5,{0x55,0xAA,0x52,0x08,0x02}},//========== page2 relative ==========
{0xEE,1,{0x01}},
{0xEF,4,{0x09,0x06,0x15,0x18}},
{0xB0,6,{0x00,0x00,0x00,0x24,0x00,0x55}},
{0x6F,1,{0x06}},
{0xB0,6,{0x00,0x77,0x00,0x94,0x00,0xC0}},
{0x6F,1,{0x0C}},
{0xB0,4,{0x00,0xE3,0x01,0x1A}},
{0xB1,6,{0x01,0x46,0x01,0x88,0x01,0xBC}},
{0x6F,1,{0x06}},
{0xB1,6,{0x02,0x0B,0x02,0x4B,0x02,0x4D}},
{0x6F,1,{0x0C}},
{0xB1,4,{0x02,0x88,0x02,0xC9}},
{0xB2,6,{0x02,0xF3,0x03,0x29,0x03,0x4E}},
{0x6F,1,{0x06}},
{0xB2,6,{0x03,0x7D,0x03,0x9B,0x03,0xBE}},
{0x6F,1,{0x0C}},
{0xB2,4,{0x03,0xD3,0x03,0xE9}},
{0xB3,4,{0x03,0xFB,0x03,0xFF}},
{0xF0,5,{0x55,0xAA,0x52,0x08,0x06}},//========== GOA relative ==========
{0xB0,2,{0x0B,0x2E}},
{0xB1,2,{0x2E,0x2E}},
{0xB2,2,{0x2E,0x09}},
{0xB3,2,{0x2A,0x29}},
{0xB4,2,{0x1B,0x19}},
{0xB5,2,{0x17,0x15}},
{0xB6,2,{0x13,0x11}},
{0xB7,2,{0x01,0x2E}},
{0xB8,2,{0x2E,0x2E}},
{0xB9,2,{0x2E,0x2E}},
{0xBA,2,{0x2E,0x2E}},
{0xBB,2,{0x2E,0x2E}},
{0xBC,2,{0x2E,0x00}},
{0xBD,2,{0x10,0x12}},
{0xBE,2,{0x14,0x16}},
{0xBF,2,{0x18,0x1A}},
{0xC0,2,{0x29,0x2A}},
{0xC1,2,{0x08,0x2E}},
{0xC2,2,{0x2E,0x2E}},
{0xC3,2,{0x2E,0x0A}},
{0xE5,2,{0x2E,0x2E}},
{0xC4,2,{0x0A,0x2E}},
{0xC5,2,{0x2E,0x2E}},
{0xC6,2,{0x2E,0x00}},
{0xC7,2,{0x2A,0x29}},
{0xC8,2,{0x10,0x12}},
{0xC9,2,{0x14,0x16}},
{0xCA,2,{0x18,0x1A}},
{0xCB,2,{0x08,0x2E}},
{0xCC,2,{0x2E,0x2E}},
{0xCD,2,{0x2E,0x2E}},
{0xCE,2,{0x2E,0x2E}},
{0xCF,2,{0x2E,0x2E}},
{0xD0,2,{0x2E,0x09}},
{0xD1,2,{0x1B,0x19}},
{0xD2,2,{0x17,0x15}},
{0xD3,2,{0x13,0x11}},
{0xD4,2,{0x29,0x2A}},
{0xD5,2,{0x01,0x2E}},
{0xD6,2,{0x2E,0x2E}},
{0xD7,2,{0x2E,0x0B}},
{0xE6,2,{0x2E,0x2E}},
{0xD8,5,{0x00,0x00,0x00,0x00,0x00}},
{0xD9,5,{0x00,0x00,0x00,0x00,0x00}},
{0xE7,1,{0x00}},
{0xF0,5,{0x55,0xAA,0x52,0x08,0x03}},// PAGE3 :
{0xB0,2,{0x20,0x00}},
{0xB1,2,{0x20,0x00}},
{0xB2,5,{0x05,0x00,0x00,0x00,0x00}},
{0xB6,5,{0x05,0x00,0x00,0x00,0x00}},
{0xB7,5,{0x05,0x00,0x00,0x00,0x00}},
{0xBA,5,{0x57,0x00,0x00,0x00,0x00}},
{0xBB,5,{0x57,0x00,0x00,0x00,0x00}},
{0xC0,4,{0x00,0x00,0x00,0x00}},
{0xC1,4,{0x00,0x00,0x00,0x00}},
{0xC4,1,{0x60}},
{0xC5,1,{0x40}},
{0xF0,5,{0x55,0xAA,0x52,0x08,0x05}},// PAGE5 :
{0xBD,5,{0x03,0x01,0x03,0x03,0x03}},
{0xB0,2,{0x17,0x06}},
{0xB1,2,{0x17,0x06}},
{0xB2,2,{0x17,0x06}},
{0xB3,2,{0x17,0x06}},
{0xB4,2,{0x17,0x06}},
{0xB5,2,{0x17,0x06}},
{0xB8,1,{0x00}},
{0xB9,1,{0x00}},
{0xBA,1,{0x00}},
{0xBB,1,{0x02}},
{0xBC,1,{0x00}},
{0xC0,1,{0x07}},
{0xC4,1,{0x80}},
{0xC5,1,{0xA4}},
{0xC8,2,{0x05,0x30}},
{0xC9,2,{0x01,0x31}},
{0xCC,3,{0x00,0x00,0x3C}},
{0xCD,3,{0x00,0x00,0x3C}},
{0xD1,5,{0x00,0x05,0x09,0x07,0x10}},
{0xD2,5,{0x00,0x05,0x0E,0x07,0x10}},
{0xE5,1,{0x06}},
{0xE6,1,{0x06}},
{0xE7,1,{0x06}},
{0xE8,1,{0x06}},
{0xE9,1,{0x06}},
{0xEA,1,{0x06}},
{0xED,1,{0x30}},
{0x6F,1,{0x11}},
{0xF3,1,{0x01}},//reload setting
{0x35,1,{0x00}},
	
{0x11, 0, {0x00}},// Normal Display
{REGFLAG_DELAY, 120, {}},

{0x29, 0, {0x00}},// Display ON
{REGFLAG_DELAY, 100, {}},
{REGFLAG_END_OF_TABLE, 0x00, {}}
};
#endif
#if  0
//update initial param for IC EK79007
static struct LCM_setting_table lcm_sleep_mode_out_setting[] = {
	/* Sleep Out */
	{0x11, 0, {} },
	{REGFLAG_DELAY, 120, {} },

	/* Display ON */
	{0x29, 0, {} },
	{REGFLAG_END_OF_TABLE, 0x00, {} }
};
#endif 
#if 1
static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	/* Sleep Mode On */
	{0x10, 0, {} },
	{REGFLAG_DELAY, 120, {} },
	
	/* Display OFF */
	{0x28, 0, {} },
	{REGFLAG_END_OF_TABLE, 0x00, {} }
};

static void push_table(struct LCM_setting_table *table, unsigned int count,
		       unsigned char force_update)
{
    unsigned int i;

    for(i = 0; i < count; i++)
    {
        unsigned cmd;
        cmd = table[i].cmd;

        switch (cmd) {

            case REGFLAG_DELAY :
                if(table[i].count <= 10)
                    MDELAY(table[i].count);
                else
                    MDELAY(table[i].count);
                break;

            case REGFLAG_END_OF_TABLE :
                break;

            default:
                dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
        }
    }
}
#endif

/**
 * LCM Driver Implementations
 */
 #if 0
static void lcm_get_gpio_infor(void)
{
	static struct device_node *node;

	node = of_find_compatible_node(NULL, NULL, "mediatek,mt6735-dispsys");

	GPIO_LCD_PWR_EN = of_get_named_gpio(node, "lcm_power_gpio", 0);
	GPIO_LCD_RST_EN = of_get_named_gpio(node, "lcm_res_gpio", 0);
	GPIO_LCD_TE_EN = of_get_named_gpio(node, "lcm_te_gpio", 0);
}

static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
#ifdef BUILD_LK
	printf("[elink][LK/LCM] %s() enter : GPIO=%d output=%d \n", __func__, GPIO, output);
#else
	printk("[elink][Kernel/LCM] %s() enter : GPIO=%d output=%d \n", __func__, GPIO, output);
#endif

	gpio_direction_output(GPIO, output);
	gpio_set_value(GPIO, output);
}
#endif

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;
		
    #if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
    #else
		params->dsi.mode   = SYNC_EVENT_VDO_MODE;		//SYNC_EVENT_VDO_MODE;
    #endif
	
		// DSI
		/* Command mode setting */
		// Three lane or Four lane
		params->dsi.LANE_NUM								= LCM_FOUR_LANE;
		
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		// Not support in MT6573
		params->dsi.packet_size=256;

		// Video mode setting		
		params->dsi.intermediat_buffer_num = 0;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		params->dsi.word_count=FRAME_WIDTH*3;
		
		params->dsi.vertical_sync_active				= 6;
		params->dsi.vertical_backporch					= 3;
		params->dsi.vertical_frontporch					= 20;
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active			= 6;
		params->dsi.horizontal_backporch				= 48;
		params->dsi.horizontal_frontporch				= 16;
		params->dsi.horizontal_active_pixel			= FRAME_WIDTH;
		
    	params->dsi.ssc_disable							= 1;
		params->dsi.PLL_CLOCK = 221;
		params->dsi.cont_clock= 1;

}

extern void DSI_clk_HS_mode(unsigned char enter);
#if 0
static void init_lcm_registers(void)
{
    unsigned int data_array[16];
    
#ifdef BUILD_LK
	printf("[elink][LK/LCM] %s() enter\n", __func__);
#else
	printk("[elink][Kernel/LCM] %s() enter\n", __func__);
#endif

	data_array[0] = 0x00003200; 
	dsi_set_cmdq( data_array, 1, 1);
	MDELAY(20);
	
	data_array[0] = 0x00110500; 
	dsi_set_cmdq( data_array, 1, 1);
	MDELAY(120);

	data_array[0] = 0x00290500; 
	dsi_set_cmdq( data_array, 1, 1);

	MDELAY(200);


#ifdef BUILD_LK
	printf("[elink][LK/LCM] %s() end\n", __func__);
#else
	printk("[elink][Kernel/LCM] %s() end\n", __func__);
#endif
}
#endif

void nt35521_wxga_early_init_lcm(void)
{
	init_lcm_gpio(GPIO146, &GPIO_LCD_RST_EN);
	init_lcm_gpio(GPIO92, &GPIO_LCD_PWR_EN);
	init_lcm_gpio(GPIO147, &GPIO_LCD_TE_EN);
	init_lcm_power("vgp3", VOL_1800);
}

static void lcm_init(void)
{
#ifdef BUILD_LK
	printf("[elink][LK/LCM] %s() enter\n", __func__);
#else
	printk("[elink][Kernel/LCM] %s() enter\n", __func__);
#endif

	//lcm_get_gpio_infor();
	nt35521_wxga_early_init_lcm();

	lcm_set_gpio_output(GPIO_LCD_PWR_EN, 0);
	lcm_set_gpio_output(GPIO_LCD_RST_EN, 0);
	lcm_set_gpio_output(GPIO_LCD_TE_EN, 0);
	MDELAY(30);
	//lcd_power_en(1);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, 1);
	MDELAY(30);
	
	lcm_set_gpio_output(GPIO_LCD_TE_EN, 1);
	MDELAY(50);
	
	lcm_set_gpio_output(GPIO_LCD_RST_EN, 1);
	MDELAY(20);
	lcm_set_gpio_output(GPIO_LCD_RST_EN, 1);
	MDELAY(20);
	lcm_set_gpio_output(GPIO_LCD_RST_EN, 1);
	MDELAY(80);
	
	//init_lcm_registers();
	//MDELAY(180);
	// when phone initial , config output high, enable backlight drv chip  
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);	
		
	
#ifdef BUILD_LK
	printf("[elink][LK/LCM] %s() end\n", __func__);
#else
	printk("[elink][Kernel/LCM] %s() end\n", __func__);
#endif

}


static void lcm_suspend(void)
{	
//	unsigned int data_array[16];
#ifdef BUILD_LK
	printf("[elink][LK/LCM] %s() enter\n", __func__);
#else
	printk("[elink][Kernel/LCM] %s() enter\n", __func__);
#endif
/* 	data_array[0] = 0x00280500;  //display off 					   
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(120);
	
	data_array[0] = 0x00100500;
	dsi_set_cmdq(data_array,1,1);//enter into sleep
	MDELAY(50); */
	
	//lcm_get_gpio_infor();
	nt35521_wxga_early_init_lcm();

	lcm_set_gpio_output(GPIO_LCD_RST_EN, 0);
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCD_TE_EN, 0);
	MDELAY(20);
	
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, 0);
	//lcd_power_en(0);
	MDELAY(20);
	push_table(lcm_deep_sleep_mode_in_setting, 
		sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
	
#ifdef BUILD_LK
	printf("[elink][LK/LCM] %s() end\n", __func__);
#else
	printk("[elink][Kernel/LCM] %s() end\n", __func__);
#endif

}


static void lcm_resume(void)
{
#ifdef BUILD_LK
	printf("[elink][LK/LCM] %s() enter\n", __func__);
#else
	printk("[elink][Kernel/LCM] %s() enter\n", __func__);
#endif

	lcm_init();


#ifdef BUILD_LK
	printf("[elink][LK/LCM] %s() end\n", __func__);
#else
	printk("[elink][Kernel/LCM] %s() end\n", __func__);
#endif
}

#if 0
static void lcm_setpwm(unsigned int divider)
{
	/* TBD */
}


static unsigned int lcm_getpwm(unsigned int divider)
{
	/* ref freq = 15MHz, B0h setting 0x80, so 80.6% * freq is pwm_clk; */
	/* pwm_clk / 255 / 2(lcm_setpwm() 6th params) = pwm_duration = 23706 */
	unsigned int pwm_clk = 23706 / (1 << divider);

	return pwm_clk;
}
#endif

LCM_DRIVER nt35521_wxga_dsi_vdo_lcm_drv = 
{
    .name			= "nt35521_wxga_dsi_vdo",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params     = lcm_get_params,
    .init           = lcm_init,
    .suspend        = lcm_suspend,
    .resume         = lcm_resume,
    //.compare_id   = lcm_compare_id,
    //.init_power	= lcm_init_power,
    //.resume_power = lcm_resume_power,
    //.suspend_power = lcm_suspend_power,
#if (LCM_DSI_CMD_MODE)
	/*.set_backlight	= lcm_setbacklight,*/
	/* .set_pwm        = lcm_setpwm, */
	/* .get_pwm        = lcm_getpwm, */
	/*.update         = lcm_update, */
#endif
   
};

