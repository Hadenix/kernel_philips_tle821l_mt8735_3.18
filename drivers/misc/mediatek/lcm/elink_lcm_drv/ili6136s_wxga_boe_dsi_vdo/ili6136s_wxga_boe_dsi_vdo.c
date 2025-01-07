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
#if 0 
 //update initial param for IC EK79007
static struct LCM_setting_table lcm_initialization_setting[] = {
	// CPT & IVO 
	{0x80, 1, {0x47}},
	{0x81, 1, {0x40}},
	{0x82, 1, {0x04}},
	{0x83, 1, {0x77}},
	{0x84, 1, {0x0f}},
	{0x85, 1, {0x70}},
	{0x86, 1, {0x70}}, 	
		
	/* Sleep out */
	{0x11, 0, {} },
	{REGFLAG_DELAY, 120, {} },
		
	/* Display ON */
	{0x29, 0, {} },
	{REGFLAG_END_OF_TABLE, 0x00, {} }
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
	
		params->physical_height = 151;
		params->physical_width	= 95;
	
		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;
	
    #if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
    #else
		params->dsi.mode   = BURST_VDO_MODE; //BURST_VDO_MODE;	//82: SYNC_EVENT_VDO_MODE
    #endif
	
		// DSI
		/* Command mode setting */
		//	//1 Three lane or Four lane
		params->dsi.LANE_NUM				= LCM_FOUR_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq	= LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding 	= LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format		= LCM_DSI_FORMAT_RGB888;
		
		params->dsi.packet_size=256;//
		params->dsi.intermediat_buffer_num = 2; //
		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		
		params->dsi.vertical_sync_active				= 4;
		params->dsi.vertical_backporch					= 4; 
		params->dsi.vertical_frontporch 				= 8; 
		params->dsi.vertical_active_line				= FRAME_HEIGHT; // 1296
		
		params->dsi.horizontal_sync_active				= 14;
		params->dsi.horizontal_backporch				= 140; 
		params->dsi.horizontal_frontporch				= 16; 
		params->dsi.horizontal_active_pixel 			= FRAME_WIDTH; // 970
		//params->dsi.pll_select=0; //0: MIPI_PLL; 1: LVDS_PLL
	
		params->dsi.PLL_CLOCK= 200; //226; //480;
}

extern void DSI_clk_HS_mode(unsigned char enter);

static void init_lcm_registers(void)
{
    unsigned int data_array[16];
    
#ifdef BUILD_LK
	printf("[elink][LK/LCM] %s() enter\n", __func__);
#else
	printk("[elink][Kernel/LCM] %s() enter\n", __func__);
#endif

			data_array[0]=0x00043902;
		data_array[1]=0x073661FF;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x0000A003;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000204;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x0000000A;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x0000000B;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x0000000C;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x0000000D;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x0000000E;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x0000000F;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000011;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000312;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000016;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000017;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000018;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000019;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x0000001A;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x0000991C;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00004023;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00001024;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00001131;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000D51;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000D52;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000C53;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000C54;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000F55;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000F56;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000E57;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000E58;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000659;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x0000075A;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x0000035B;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x0000035C;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x0000035D;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x0000035E;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x0000035F;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000360;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000D67;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000D68;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000C69;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000C6A;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000F6B;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000F6C;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000E6D;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000E6E;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x0000066F;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000770;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);	
		data_array[0]=0x00023902;
		data_array[1]=0x00000371;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000372;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000373;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000374;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000375;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000376;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00000183;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00043902;
		data_array[1]=0x083661FF;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x0000428B;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00043902;
		data_array[1]=0x013661FF;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;///////////////////add
		data_array[1]=0x0001063;
		dsi_set_cmdq( data_array,2,1);////////////////////
		MDELAY(1);
	
		data_array[0]=0x00043902;
		data_array[1]=0x013661FF;
		dsi_set_cmdq( data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00001931;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00043902;
		data_array[1]=0x013661FF;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x0000A950;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		/*
		data_array[0]=0x00043902;
		data_array[1]=0x013661FF;
		dsi_set_cmdq( data_array,2,1);
		MDELAY(10);
		data_array[0]=0x00023902;
		data_array[1]=0x0000A451;
		dsi_set_cmdq( data_array,2,1);
		MDELAY(10);
	
		data_array[0]=0x00043902;
		data_array[1]=0x013661FF;
		dsi_set_cmdq( data_array,2,1);
		MDELAY(10);
	
		data_array[0]=0x00023902;
		data_array[1]=0x00000390;
		dsi_set_cmdq( data_array,2,1);
		MDELAY(10);
	
		data_array[0]=0x00043902;
		data_array[1]=0x013661FF;
		dsi_set_cmdq( data_array,2,1);
		MDELAY(10);
	
		data_array[0]=0x00023902;
		data_array[1]=0x0000A950;
		dsi_set_cmdq( data_array,2,1);
		MDELAY(10);*/
		
		data_array[0]=0x00043902;
		data_array[1]=0x013661FF;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x0000A451;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00043902;
		data_array[1]=0x013661FF;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00009653;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00043902;
		data_array[1]=0x013661FF;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000020A0;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000033A1;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00003FA2;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000010A3;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000014A4;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000023A5;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00001AA6;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00001DA7;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x0000A1A8;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000017A9;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000022AA;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000089AB;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00001AAC;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000018AD;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00004DAE;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000022AF;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000025B0;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00004EB1;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00005BB2;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00002EB3;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00043902;
		data_array[1]=0x013661FF;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000021C0;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000033C1;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00003EC2;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000014C3;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000010C4;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000025C5;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000017C6;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000020C7;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00009CC8;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00001DC9;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000029CA;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00008CCB;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000019CC;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000018CD;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00004DCE;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000024CF;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000026D0;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000050D1;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x00005DD2;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000031D3;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00043902;
		data_array[1]=0x083661FF;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00023902;
		data_array[1]=0x000024AB;
		dsi_set_cmdq(data_array,2,1);
		MDELAY(1);
		
		data_array[0]=0x00043902;
		data_array[1]=0x003661FF;
		dsi_set_cmdq( data_array,2,1);
		MDELAY(1);
	
		data_array[0] = 0x00110500; //0x11,exit sleep mode
		dsi_set_cmdq( data_array, 1, 1);
		MDELAY(120);
	
		data_array[0] = 0x00290500; //0x29,display on mode
		dsi_set_cmdq( data_array, 1, 1);
		MDELAY(30);


#ifdef BUILD_LK
	printf("[elink][LK/LCM] %s() end\n", __func__);
#else
	printk("[elink][Kernel/LCM] %s() end\n", __func__);
#endif
}


void ili6136s_boe_early_init_lcm(void)
{
	init_lcm_gpio(GPIO146, &GPIO_LCD_RST_EN);
	init_lcm_gpio(GPIO92, &GPIO_LCD_PWR_EN);
	init_lcm_gpio(GPIO124, &GPIO_LCD_TE_EN);
//	init_lcm_power("vgp3", VOL_1800);
}

static void lcm_init(void)
{
#ifdef BUILD_LK
	printf("[elink][LK/LCM] %s() enter\n", __func__);
#else
	printk("[elink][Kernel/LCM] %s() enter\n", __func__);
#endif

	//lcm_get_gpio_infor();
	ili6136s_boe_early_init_lcm();

	lcm_set_gpio_output(GPIO_LCD_PWR_EN, 0);
	lcm_set_gpio_output(GPIO_LCD_RST_EN, 0);
	lcm_set_gpio_output(GPIO_LCD_TE_EN, 0);
	MDELAY(30);
	//lcd_power_en(1);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, 1);
	MDELAY(80);
	
	lcm_set_gpio_output(GPIO_LCD_TE_EN, 1);
	MDELAY(50);
	
	lcm_set_gpio_output(GPIO_LCD_RST_EN, 1);
	MDELAY(20);
	lcm_set_gpio_output(GPIO_LCD_RST_EN, 1);
	MDELAY(20);
	lcm_set_gpio_output(GPIO_LCD_RST_EN, 1);
	MDELAY(120);
	
	init_lcm_registers();
	MDELAY(180);
	// when phone initial , config output high, enable backlight drv chip  
	//push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);	
		
	
#ifdef BUILD_LK
	printf("[elink][LK/LCM] %s() end\n", __func__);
#else
	printk("[elink][Kernel/LCM] %s() end\n", __func__);
#endif

}


static void lcm_suspend(void)
{	
	unsigned int data_array[16];
#ifdef BUILD_LK
	printf("[elink][LK/LCM] %s() enter\n", __func__);
#else
	printk("[elink][Kernel/LCM] %s() enter\n", __func__);
#endif

	data_array[0] = 0x00280500; 	
	dsi_set_cmdq(data_array, 1, 1);	
	MDELAY(40); 	
	
	data_array[0] = 0x00100500; 
	dsi_set_cmdq(data_array, 1, 1);
	
	
	//lcm_get_gpio_infor();
	ili6136s_boe_early_init_lcm();
	
	lcm_set_gpio_output(GPIO_LCD_RST_EN, 0);
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCD_TE_EN, 0);
	MDELAY(20);
	
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, 0);
	//lcd_power_en(0);
	MDELAY(20);
/* 	push_table(lcm_deep_sleep_mode_in_setting, 
		sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1); */
	
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


LCM_DRIVER ili6136s_wxga_boe_dsi_vdo_lcm_drv = 
{
    .name			= "ili6136s_wxga_boe_dsi_vdo",
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

