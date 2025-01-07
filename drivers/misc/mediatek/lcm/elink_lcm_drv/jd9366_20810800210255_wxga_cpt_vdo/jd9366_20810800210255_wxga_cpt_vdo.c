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

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

/**
 * Local Constants
 */
#define FRAME_WIDTH			(800)
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
//Page0
{0xE0,1,{0x00}}, 

//--- PASSWORD  ----//
{0xE1,1,{0x93}}, 
{0xE2,1,{0x65}}, 
{0xE3,1,{0xF8}}, 

{0xE6,1,{0x02}}, 
{0xE7,1,{0x02}}, 

//--- Page1  ----//
{0xE0,1,{0x01}}, 

//Set VCOM
{0x00,1,{0x00}}, 
{0x01,1,{0xB7}}, 

//Set Gamma Power)
{0x17,1,{0x00}}, 
{0x18,1,{0xB2}}, 
{0x19,1,{0x01}}, 
{0x1A,1,{0x00}}, 
{0x1B,1,{0xB2}},   
{0x1C,1,{0x01}}, 

//Set Gate Power
{0x1F,1,{0x7E}}, 
{0x20,1,{0x28}}, 	
{0x21,1,{0x28}}, 
{0x22,1,{0x0E}}, 
{0x24,1,{0xC8}},   //0x38,1,{ [7]:VGH_EN=1,1,{ [6]:VGL_EN=1,1,{[5]:AVDD_EN=0,1,{[4]:AVEE_EN=0


//SET RGBCYC
{0x37,1,{0x29}}, 	//[5:4]ENZ[1:0]=10,1,{ [3]SS=1,1,{ [0]BGR=1
{0x38,1,{0x05}}, 	//JDT=101 Zigzag inversion
{0x39,1,{0x08}}, 	//RGB_N_EQ1,1,{ modify 20140806
{0x3A,1,{0x12}}, 	//RGB_N_EQ2,1,{ modify 20140806
{0x3C,1,{0x78}}, 	//SET EQ3 for TE_H
{0x3E,1,{0xFF}}, 	//SET CHGEN_ON,1,{ modify 20140827 
{0x3E,1,{0xFF}}, 	//SET CHGEN_OFF,1,{ modify 20140827 
{0x3F,1,{0xFF}}, 	//SET CHGEN_OFF2,1,{ modify 20140827

//Set TCON
{0x40,1,{0x06}}, 	//RSO=800 Pixels
{0x41,1,{0xA0}}, 	//LN=640->1280 line
{0x43,1,{0x08}}, 	//VFP
{0x44,1,{0x07}}, 	//VBP
{0x45,1,{0x24}}, 	//HBP

//--- power voltage  ----//
{0x55,1,{0x01}}, 	//DCDCM=1111,1,{ External pwoer ic
{0x56,1,{0x01}}, 
{0x57,1,{0x89}}, 
{0x58,1,{0x0A}}, 
{0x59,1,{0x2A}}, 	//VCL = -2.9V
{0x5A,1,{0x31}}, 	//VGH = 19V
{0x5B,1,{0x15}}, 	//VGL = -11V

//--- Gamma  ----//
{0x5D,1,{0x7C}}, 
{0x5E,1,{0x59}}, 
{0x5F,1,{0x47}}, 
{0x60,1,{0x39}}, 
{0x61,1,{0x33}}, 
{0x62,1,{0x23}}, 
{0x63,1,{0x27}}, 
{0x64,1,{0x10}}, 
{0x65,1,{0x27}}, 
{0x66,1,{0x25}}, 
{0x67,1,{0x23}}, 
{0x68,1,{0x3F}}, 
{0x69,1,{0x2C}}, 
{0x6A,1,{0x33}}, 
{0x6B,1,{0x25}}, 
{0x6C,1,{0x23}}, 
{0x6D,1,{0x17}}, 
{0x6E,1,{0x0A}}, 
{0x6F,1,{0x00}}, 
{0x70,1,{0x7C}}, 
{0x71,1,{0x59}}, 
{0x72,1,{0x47}}, 
{0x73,1,{0x39}}, 
{0x74,1,{0x33}}, 
{0x75,1,{0x23}}, 
{0x76,1,{0x27}}, 
{0x77,1,{0x10}}, 
{0x78,1,{0x27}}, 
{0x79,1,{0x25}}, 
{0x7A,1,{0x23}}, 
{0x7B,1,{0x3F}}, 
{0x7C,1,{0x2C}}, 
{0x7D,1,{0x33}}, 
{0x7E,1,{0x25}}, 
{0x7F,1,{0x23}}, 
{0x80,1,{0x17}}, 
{0x81,1,{0x0A}}, 
{0x82,1,{0x00}}, 

//Page2,1,{ for GIP
{0xE0,1,{0x02}}, 

//GIP_L Pin mapping
{0x00,1,{0x00}}, 
{0x01,1,{0x04}},    
{0x02,1,{0x08}},  
{0x03,1,{0x05}},  
{0x04,1,{0x09}},  
{0x05,1,{0x06}},  
{0x06,1,{0x0A}},  
{0x07,1,{0x07}},  
{0x08,1,{0x0B}},  
{0x09,1,{0x1f}},  
{0x0A,1,{0x1f}},  
{0x0B,1,{0x1f}},  
{0x0C,1,{0x1f}},  
{0x0D,1,{0x1f}},  
{0x0E,1,{0x1f}},  
{0x0F,1,{0x17}},  
{0x10,1,{0x37}},  
{0x11,1,{0x10}},  
{0x12,1,{0x1F}},  
{0x13,1,{0x1F}},  
{0x14,1,{0x1F}},  
{0x15,1,{0x1F}},  

//GIP_R Pin mapping
{0x16,1,{0x00}}, 
{0x17,1,{0x04}},    
{0x18,1,{0x08}},  
{0x19,1,{0x05}},  
{0x1A,1,{0x09}},  
{0x1B,1,{0x06}},  
{0x1C,1,{0x0A}},  
{0x1D,1,{0x07}},  
{0x1E,1,{0x0B}},  
{0x1F,1,{0x1f}},  
{0x20,1,{0x1f}},  
{0x21,1,{0x1f}},  
{0x22,1,{0x1f}},  
{0x23,1,{0x1f}},  
{0x24,1,{0x1f}},  
{0x25,1,{0x17}},  
{0x26,1,{0x37}},  
{0x27,1,{0x10}},  
{0x28,1,{0x1F}},  
{0x29,1,{0x1F}},  
{0x2A,1,{0x1F}},  
{0x2B,1,{0x1F}},  

//GIP Timing  
{0x58,1,{0x01}},  
{0x59,1,{0x00}},  
{0x5A,1,{0x00}},  
{0x5B,1,{0x00}},  
{0x5C,1,{0x01}},  
{0x5D,1,{0x30}},  
{0x5E,1,{0x00}},  
{0x5F,1,{0x00}},  
{0x60,1,{0x30}},  
{0x61,1,{0x00}},  
{0x62,1,{0x00}},  
{0x63,1,{0x03}},  //STV_ON
{0x64,1,{0x6A}},  //STV_OFF
{0x65,1,{0x45}},  
{0x66,1,{0x08}},  
{0x67,1,{0x73}},  
{0x68,1,{0x05}},  
{0x69,1,{0x06}},  //CKV_ON
{0x6A,1,{0x6A}},  //CKV_OFF
{0x6B,1,{0x08}},  
{0x6C,1,{0x00}},  
{0x6D,1,{0x00}},  
{0x6E,1,{0x00}},  
{0x6F,1,{0x88}},  
{0x70,1,{0x00}},  
{0x71,1,{0x00}},  
{0x72,1,{0x06}},  
{0x73,1,{0x7B}},  
{0x74,1,{0x00}},  
{0x75,1,{0x80}},  
{0x76,1,{0x00}},  
{0x77,1,{0x05}},  
{0x78,1,{0x10}},  
{0x79,1,{0x00}},  
{0x7A,1,{0x00}},  
{0x7B,1,{0x00}},  
{0x7C,1,{0x00}},  
{0x7D,1,{0x03}},  
{0x7E,1,{0x7B}},  

{0xE0,1,{0x01}}, 
{0x0E,1,{0x01}}, 	//LEDON output VCSW2
//Page3
{0xE0,1,{0x03}}, 
{0x98,1,{0x2F}}, 	//From 2E to 3E,1,{ LED ON

//Page4
{0xE0,1,{0x04}}, 
{0x2B,1,{0x2B}}, 
{0x2E,1,{0x44}}, 

//Page0
{0xE0,1,{0x00}}, 	
		
{0x11, 0, {0x00}},// Normal Display
{REGFLAG_DELAY, 120, {}},

{0x29, 0, {0x00}},// Display ON
{0x35, 0, {0x00}},//TE
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
#if 0
static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	/* Sleep Mode On */
	{0x10, 0, {} },
	{REGFLAG_DELAY, 120, {} },
	
	/* Display OFF */
	{0x28, 0, {} },
	{REGFLAG_END_OF_TABLE, 0x00, {} }
};
#endif
#if 1
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
		
	params->dsi.vertical_sync_active				= 4;
	params->dsi.vertical_backporch					= 4;
	params->dsi.vertical_frontporch					= 8;
	params->dsi.vertical_active_line				= FRAME_HEIGHT; 

	params->dsi.horizontal_sync_active				= 20;
	params->dsi.horizontal_backporch				= 20;
	params->dsi.horizontal_frontporch				= 20;
	params->dsi.horizontal_active_pixel			= FRAME_WIDTH;

	params->dsi.PLL_CLOCK=230; 	

    params->dsi.esd_check_enable = 1;
    params->dsi.customization_esd_check_enable = 1;
	params->dsi.lcm_esd_check_table[0].cmd          = 0x0a;
	params->dsi.lcm_esd_check_table[0].count        = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9c;
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

static void jd9366_wxga_early_init_lcm(void)
{
	init_lcm_gpio(GPIO146, &GPIO_LCD_RST_EN);
	init_lcm_gpio(GPIO92, &GPIO_LCD_PWR_EN);
	init_lcm_gpio(GPIO147, &GPIO_LCD_TE_EN);
	init_lcm_power("vrf18_1", VOL_1800);
}

static void lcm_init(void)
{
#ifdef BUILD_LK
	printf("[elink][LK/LCM] %s() enter\n", __func__);
#else
	printk("[elink][Kernel/LCM] %s() enter\n", __func__);
#endif

	//lcm_get_gpio_infor();
	jd9366_wxga_early_init_lcm();

lcd_power_en(0);	
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, 0);
	lcm_set_gpio_output(GPIO_LCD_RST_EN, 0);
	lcm_set_gpio_output(GPIO_LCD_TE_EN, 0);
	MDELAY(30);
	
lcd_power_en(1);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, 1);
	MDELAY(30);
	
	lcm_set_gpio_output(GPIO_LCD_TE_EN, 1);
	MDELAY(50);
	
	lcm_set_gpio_output(GPIO_LCD_RST_EN, 1);
	MDELAY(20);
	lcm_set_gpio_output(GPIO_LCD_RST_EN, 0);
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

#if 0
static void test(void)
{

     unsigned int array[4]={0};

     char  buffer[6]={0};

     //int id=0;

     char id1 =0;
     //char id2 =0;
     //char id3 =0;

     char i=0;
     array[0] = 0x93E11500;
     dsi_set_cmdq(array, 2, 1);
     array[0] = 0x65E21500;
     dsi_set_cmdq(array, 2, 1);
     array[0] = 0xF8E31500;
     dsi_set_cmdq(array, 2, 1);
     array[0] = 0x02E01500;
     dsi_set_cmdq(array, 2, 1);
     MDELAY(10);


             array[0] = 0x00013700;//0x00023700;
             MDELAY(5);
             dsi_set_cmdq(array, 1, 1);
             MDELAY(5);
             read_reg_v2(0x00, buffer, 1);
             MDELAY(5);
             id1= buffer[0];
             printk("lhh  page 2 reg==%x\n",i);
             printk("lhh  page 2 var==%x\n",id1);

     for( i=0;i<0x7F;i++)
     {
             array[0] = 0x00013700;//0x00023700;
             MDELAY(5);
             dsi_set_cmdq(array, 1, 1);
             MDELAY(5);
             read_reg_v2(i, buffer, 1);
             MDELAY(5);
             id1= buffer[0];
             printk("lhh  page 2 reg==%x\n",i);
             printk("lhh  page 2 var==%x\n",id1);
     }
	 
	printk("=================================================\n"); 
	 
	 i =0;
     array[0] = 0x01E01500;
     dsi_set_cmdq(array, 2, 1);
     MDELAY(10);


             array[0] = 0x00013700;//0x00023700;
             MDELAY(5);
             dsi_set_cmdq(array, 1, 1);
             MDELAY(5);
             read_reg_v2(0x00, buffer, 1);
             MDELAY(5);
             id1= buffer[0];
             printk("lhh  page 1 reg==%x\n",i);
             printk("lhh  page 1 var==%x\n",id1);

     for( i=0;i<0x83;i++)
     {
             array[0] = 0x00013700;//0x00023700;
             MDELAY(5);
             dsi_set_cmdq(array, 1, 1);
             MDELAY(5);
             read_reg_v2(i, buffer, 1);
             MDELAY(5);
             id1= buffer[0];
             printk("lhh  page 1 reg==%x\n",i);
             printk("lhh  page 1 var==%x\n",id1);
     }
}
#endif

static void lcm_suspend(void)
{
	unsigned int data_array[16];
#ifdef BUILD_LK
	printf("[elink][LK/LCM] %s() enter\n", __func__);
#else
	printk("[elink][Kernel/LCM] %s() enter\n", __func__);
#endif

	//test();

	MDELAY(5);
 	data_array[0] = 0x00280500;  //display off 					   
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(20);
	
	data_array[0] = 0x00100500;
	dsi_set_cmdq(data_array,1,1);//enter into sleep
	MDELAY(120);
	
	
	//lcm_get_gpio_infor();
	jd9366_wxga_early_init_lcm();

	lcm_set_gpio_output(GPIO_LCD_RST_EN, 0);
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCD_TE_EN, 0);
	MDELAY(20);
	
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, 0);
	lcd_power_en(0);
	MDELAY(20);
	// push_table(lcm_deep_sleep_mode_in_setting, 
		// sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
	
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

// return TRUE: need recovery
// return FALSE: No need recovery
static unsigned int lcm_esd_check(void)
{
#ifndef BUILD_LK
	char  buffer[3];
	int   array[4];

	array[0] = 0x00013700;
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0x0A, buffer, 1);

	if(buffer[0] != 0x9C)
	{
		printk("[LCM ERROR] [0x53]=0x%02x\n", buffer[0]);
		return TRUE;
	}
	else
	{
		printk("[LCM NORMAL] [0x53]=0x%02x\n", buffer[0]);
		return FALSE;
	}
#else
	return FALSE;
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

LCM_DRIVER jd9366_20810800210255_wxga_cpt_vdo_lcm_drv = 
{
    .name			= "jd9366_20810800210255_wxga_cpt_vdo",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params     = lcm_get_params,
    .init           = lcm_init,
    .suspend        = lcm_suspend,
    .resume         = lcm_resume,
	.esd_check      = lcm_esd_check,
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

