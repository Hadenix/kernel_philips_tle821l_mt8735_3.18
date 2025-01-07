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
#define FRAME_WIDTH		(720)
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

	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	params->physical_height = 133;
	params->physical_width 	= 75;
	
	// enable tearing-free ****
	params->dbi.te_mode 				= LCM_DBI_TE_MODE_DISABLED; //LCM_DBI_TE_MODE_VSYNC_OR_HSYNC;//LCM_DBI_TE_MODE_DISABLED;//LCM_DBI_TE_MODE_VSYNC_ONLY;
	params->dbi.te_edge_polarity		= LCM_POLARITY_FALLING;

#if (LCM_DSI_CMD_MODE)
	params->dsi.mode   = CMD_MODE;
#else
	params->dsi.mode   = BURST_VDO_MODE; //SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE; 
#endif
	
	// DSI
	/* Command mode setting */
	params->dsi.LANE_NUM				= LCM_FOUR_LANE;//LCM_TWO_LANE;LCM_FOUR_LANE
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding	  	= LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format		= LCM_DSI_FORMAT_RGB888;
	

	// Highly depends on LCD driver capability.
	// Not support in MT6573
	params->dsi.packet_size=256;

	// Video mode setting		
	params->dsi.intermediat_buffer_num = 2; //because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	
	params->dsi.word_count=FRAME_WIDTH*3; 

	
	// 5.94′ 
	params->dsi.vertical_sync_active = 2;//2;
	params->dsi.vertical_backporch	 = 14;
	params->dsi.vertical_frontporch	 = 16;	
	// params->dsi.vertical_blanking_pixel	 = 60;
	params->dsi.vertical_active_line	 = FRAME_HEIGHT; // 1312

	params->dsi.horizontal_sync_active	 = 4;
	params->dsi.horizontal_backporch	 = 42;
	params->dsi.horizontal_frontporch	 = 44;
	params->dsi.horizontal_blanking_pixel= 50;
	params->dsi.horizontal_active_pixel	 = FRAME_WIDTH; // 810

	params->dsi.PLL_CLOCK=192;
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

    // 维特盛世 720*1280 ,  TM 玻璃,OTM1283A


	data_array[0] = 0x00001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00043902;
	data_array[1] = 0x018312FF;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x80001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00033902;
	data_array[1] = 0x008312FF;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x92001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00033902;
	data_array[1] = 0x000230FF;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x80001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x000A3902;
	data_array[1] = 0x006400C0;
	data_array[2] = 0x6400120E;
	data_array[3] = 0x0000120E;
	dsi_set_cmdq(data_array, 4, 1);
	MDELAY(1);
	
	data_array[0] = 0xB4001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000055C0;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x81001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000055C1;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x81001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000082C4;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x90001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000049C4;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x82001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000002C4;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0xC6001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000003B0;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0xA0001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x000F3902;
	data_array[1] = 0x061005C4;
	data_array[2] = 0x10150502;
	data_array[3] = 0x02071005;
	data_array[4] = 0x00101505;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	
	data_array[0] = 0xB0001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00033902;
	data_array[1] = 0x000000C4;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0xBB001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000080C5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x91001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00033902;
	data_array[1] = 0x005046C5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00033902;
	data_array[1] = 0x00AEAED8;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000080D9;//
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0xB0001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00033902;
	data_array[1] = 0x00B804C5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x90001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00053902;
	data_array[1] = 0x021102F5;
	data_array[2] = 0x00000011;
	dsi_set_cmdq(data_array, 3, 1);
	MDELAY(1);
	
	data_array[0] = 0x90001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000050C5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x94001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000066C5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0xB2001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00033902;
	data_array[1] = 0x000000F5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0xB4001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00033902;
	data_array[1] = 0x000000F5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0xB6001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00033902;
	data_array[1] = 0x000000F5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0xB8001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00033902;
	data_array[1] = 0x000000F5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x94001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000002F5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0xBA001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000003F5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0xB2001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000040C5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0xB4001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000C0C5;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x80001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x000C3902;
	data_array[1] = 0x000000CB;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	dsi_set_cmdq(data_array, 4, 1);
	MDELAY(1);
	
	data_array[0] = 0x90001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00103902;
	data_array[1] = 0x000000CB;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	
	data_array[0] = 0xA0001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00103902;
	data_array[1] = 0x000000CB;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	
	data_array[0] = 0xB0001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00103902;
	data_array[1] = 0x000000CB;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	
	data_array[0] = 0xC0001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00103902;
	data_array[1] = 0x050505CB;
	data_array[2] = 0x00050505;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	
	data_array[0] = 0xD0001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00103902;
	data_array[1] = 0x000000CB;
	data_array[2] = 0x05050500;
	data_array[3] = 0x05050505;
	data_array[4] = 0x00000505;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	
	data_array[0] = 0xE0001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x000F3902;
	data_array[1] = 0x000000CB;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00050505;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	
	data_array[0] = 0xF0001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x000C3902;
	data_array[1] = 0xFFFFFFCB;
	data_array[2] = 0xFFFFFFFF;
	data_array[3] = 0xFFFFFFFC;
	dsi_set_cmdq(data_array, 4, 1);
	MDELAY(1);
	
	data_array[0] = 0x80001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00103902;
	data_array[1] = 0x100A0CCC;
	data_array[2] = 0x0004020E;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	
	data_array[0] = 0x90001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00103902;
	data_array[1] = 0x000000CC;
	data_array[2] = 0x2D2E0600;
	data_array[3] = 0x0D0F090B;
	data_array[4] = 0x00000301;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	
	data_array[0] = 0xA0001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x000F3902;
	data_array[1] = 0x000000CC;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x002D2E05;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	
	data_array[0] = 0xB0001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00103902;
	data_array[1] = 0x090F0DCC;
	data_array[2] = 0x0001030B;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	
	data_array[0] = 0xC0001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00103902;
	data_array[1] = 0x000000CC;
	data_array[2] = 0x2E2D0600;
	data_array[3] = 0x0C0A100E;
	data_array[4] = 0x00000204;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	
	data_array[0] = 0xD0001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x000F3902;
	data_array[1] = 0x000000CC;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x002E2D05;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	
	data_array[0] = 0x80001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x000D3902;
	data_array[1] = 0x18038BCE;
	data_array[2] = 0x8918038A;
	data_array[3] = 0x03881803;
	data_array[4] = 0x00000018;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	
	data_array[0] = 0x90001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x000F3902;
	data_array[1] = 0x180F38CE;
	data_array[2] = 0x00180E38;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	
	data_array[0] = 0xA0001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x000F3902;
	data_array[1] = 0x050738CE;
	data_array[2] = 0x00180000;
	data_array[3] = 0x01050638;
	data_array[4] = 0x00001800;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	
	data_array[0] = 0xB0001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x000F3902;
	data_array[1] = 0x050538CE;
	data_array[2] = 0x00180002;
	data_array[3] = 0x03050438;
	data_array[4] = 0x00001800;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	
	data_array[0] = 0xC0001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x000F3902;
	data_array[1] = 0x050338CE;
	data_array[2] = 0x00180004;
	data_array[3] = 0x05050238;
	data_array[4] = 0x00001800;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	
	data_array[0] = 0xD0001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x000F3902;
	data_array[1] = 0x050138CE;
	data_array[2] = 0x00180006;
	data_array[3] = 0x07050038;
	data_array[4] = 0x00001800;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	
	data_array[0] = 0x80001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x000F3902;
	data_array[1] = 0x000000CF;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	
	data_array[0] = 0x90001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x000F3902;
	data_array[1] = 0x000000CF;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	
	data_array[0] = 0xA0001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x000F3902;
	data_array[1] = 0x000000CF;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	
	data_array[0] = 0xB0001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x000F3902;
	data_array[1] = 0x000000CF;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	data_array[4] = 0x00000000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);
	
	data_array[0] = 0xC0001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x000C3902;
	data_array[1] = 0x200101CF;
	data_array[2] = 0x02000020;
	data_array[3] = 0x08030081;
	dsi_set_cmdq(data_array, 4, 1);
	MDELAY(1);
	
	data_array[0] = 0xB5001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00073902;
	data_array[1] = 0x7F1138C5;
	data_array[2] = 0x007F1138;
	dsi_set_cmdq(data_array, 3, 1);
	MDELAY(1);
	
	data_array[0] = 0x00001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00113902;
	data_array[1] = 0x160F00E1;
	data_array[2] = 0x0C11070E;
	data_array[3] = 0x0B06020B;
	data_array[4] = 0x090F0F08;
	data_array[5] = 0x00000000;
	dsi_set_cmdq(data_array, 6, 1);
	MDELAY(1);
	
	data_array[0] = 0x00001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00113902;
	data_array[1] = 0x160F00E2;
	data_array[2] = 0x0C11070E;
	data_array[3] = 0x0B06020B;
	data_array[4] = 0x090F0F08;
	data_array[5] = 0x00000000;
	dsi_set_cmdq(data_array, 6, 1);
	MDELAY(1);
	
	data_array[0] = 0x00001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00043902;
	data_array[1] = 0xFFFFFFFF;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00110500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(150);
	
	data_array[0] = 0x00290500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(20);


#ifdef BUILD_LK
	printf("[elink][LK/LCM] %s() end\n", __func__);
#else
	printk("[elink][Kernel/LCM] %s() end\n", __func__);
#endif
}


void otm1283_hd720_early_init_lcm(void)
{
	init_lcm_gpio(GPIO146, &GPIO_LCD_RST_EN);
	init_lcm_gpio(GPIO80, &GPIO_LCD_PWR_EN);
	init_lcm_gpio(GPIO147, &GPIO_LCD_TE_EN);
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
	otm1283_hd720_early_init_lcm();

	lcm_set_gpio_output(GPIO_LCD_PWR_EN, 0);
	lcm_set_gpio_output(GPIO_LCD_RST_EN, 0);
	lcm_set_gpio_output(GPIO_LCD_TE_EN, 0);
	MDELAY(30);
	//lcd_power_en(1);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, 1);
	MDELAY(30);
	
	lcm_set_gpio_output(GPIO_LCD_TE_EN, 1);
	MDELAY(50);
	
//	lcm_set_gpio_output(GPIO_LCD_RST_EN, 1);
//	MDELAY(20);
//	lcm_set_gpio_output(GPIO_LCD_RST_EN, 1);
//	MDELAY(20);
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
	data_array[0] = 0x00280500;  //display off 					   
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(20);
	
	data_array[0] = 0x00100500;
	dsi_set_cmdq(data_array,1,1);//enter into sleep
	MDELAY(100);
	
	
	//lcm_get_gpio_infor();
	otm1283_hd720_early_init_lcm();
	
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


LCM_DRIVER otm1283_hd720_dsi_vdo_tm_lcm_drv = 
{
    .name			= "otm1283_hd720_dsi_vdo_tm",
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

