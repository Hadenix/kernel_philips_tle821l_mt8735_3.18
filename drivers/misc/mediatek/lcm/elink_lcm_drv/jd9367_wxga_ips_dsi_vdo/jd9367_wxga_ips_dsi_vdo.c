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
#define FRAME_WIDTH (800)
#define FRAME_HEIGHT (1280)

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
 #if  0
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

//update initial param for IC EK79007
static struct LCM_setting_table lcm_sleep_mode_out_setting[] = {
	/* Sleep Out */
	{0x11, 0, {} },
	{REGFLAG_DELAY, 120, {} },

	/* Display ON */
	{0x29, 0, {} },
	{REGFLAG_END_OF_TABLE, 0x00, {} }
};


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


	params->dsi.mode   = BURST_VDO_MODE;

	// DSI
	/* Command mode setting */
	params->dsi.LANE_NUM				= LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine.
	//params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	//params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	//params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

	// Highly depends on LCD driver capability.
	// Not support in MT6573
	//params->dsi.packet_size=256;

	// Video mode setting		
	//params->dsi.intermediat_buffer_num = 2; //because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	//params->dsi.word_count=800*3;	


	params->dsi.vertical_sync_active 	= 4;  
	params->dsi.vertical_backporch 		= 10;  
	params->dsi.vertical_frontporch 	= 30;  
	params->dsi.vertical_active_line = FRAME_HEIGHT; //1324 

	params->dsi.horizontal_sync_active 	= 8; 
	params->dsi.horizontal_backporch 	= 8;  
	params->dsi.horizontal_frontporch 	= 32;  
	params->dsi.horizontal_active_pixel = FRAME_WIDTH; //848

	params->dsi.PLL_CLOCK=202; //

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
	

	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000000E0;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000093E1;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000065E2;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000F8E3;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000000E0;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001070;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001371;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000672;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000001E0;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00001500;
	dsi_set_cmdq( data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00007001;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000003;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00006F04;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000017;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000D718;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000519;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000001A;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000D71B;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000051C;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000791F;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002D20;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002D21;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000E22;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000F824;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000F126;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000937;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000438;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000C39;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000183A;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000783C;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000640;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000A041;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000155;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000156;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000A957;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000A58;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002A59;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000375A;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000195B;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000795D;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000575E;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00003F5F;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00003160;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002D61;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001D62;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002263;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000C64;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002565;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002466;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002467;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004168;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002F69;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000366A;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000286B;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000266C;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001C6D;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000086E;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000026F;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00006770;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004971;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00003F72;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00003173;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002D74;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001D75;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002276;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000C77;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002578;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002479;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000247A;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000417B;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002F7C;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000367D;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000287E;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000267F;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001C80;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000881;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000282;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000002E0;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00001500;
	dsi_set_cmdq( data_array, 1, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000401;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000602;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000803;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000A04;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000C05;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000E06;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001707;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001808;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F09;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000100A;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F0B;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F0C;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F0D;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F0E;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F0F;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F10;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F11;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F12;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001213;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F14;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F15;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000116;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000517;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000718;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000919;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000B1A;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000D1B;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000F1C;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000171D;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000181E;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F1F;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001120;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F21;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F22;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F23;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F24;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F25;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F26;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F27;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F28;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001329;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F2A;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F2B;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000112C;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000F2D;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000D2E;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000B2F;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000930;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000731;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000532;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001833;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001734;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F35;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000136;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F37;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F38;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F39;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F3A;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F3B;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F3C;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F3D;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F3E;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000133F;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F40;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F41;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001042;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000E43;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000C44;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000A45;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000846;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000647;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000448;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001849;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000174A;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F4B;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000004C;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F4D;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F4E;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F4F;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F50;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F51;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F52;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F53;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F54;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001255;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F56;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001F57;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001058;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000059;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000005A;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000105B;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000075C;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000305D;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000005E;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000005F;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00003060;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000361;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000462;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000363;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00007364;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00007565;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000D66;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000B367;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000968;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000669;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000736A;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000046B;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000006C;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000046D;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000046E;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000886F;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000070;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000071;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000672;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00007B73;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000074;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000BC75;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000076;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000D77;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002C78;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000079;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000007A;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000007B;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000007C;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000037D;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00007B7E;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000004E0;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002B2B;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000442E;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000000E0;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000002E6;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00023902;
	data_array[1] = 0x000002E7;
	dsi_set_cmdq( data_array, 2, 1);
	MDELAY(1);
	
	data_array[0] = 0x00110500;
	dsi_set_cmdq( data_array, 1, 1);
	MDELAY(150);
	
	data_array[0] = 0x00290500;
	dsi_set_cmdq( data_array, 1, 1);
	MDELAY(20);
	
#ifdef BUILD_LK
	printf("[elink][LK/LCM] %s() end\n", __func__);
#else
	printk("[elink][Kernel/LCM] %s() end\n", __func__);
#endif
}


static void early_init_lcm(void)
{
	init_lcm_gpio(GPIO146, &GPIO_LCD_RST_EN);
	init_lcm_gpio(GPIO92, &GPIO_LCD_PWR_EN);
	init_lcm_gpio(GPIO147, &GPIO_LCD_TE_EN);
	//init_lcm_power("vgp3", VOL_1800);
}

static void lcm_init(void)
{
#ifdef BUILD_LK
	printf("[elink][LK/LCM] %s() enter\n", __func__);
#else
	printk("[elink][Kernel/LCM] %s() enter\n", __func__);
#endif

	//lcm_get_gpio_infor();
	early_init_lcm();

	lcm_set_gpio_output(GPIO_LCD_PWR_EN, 0);
	lcm_set_gpio_output(GPIO_LCD_RST_EN, 0);
	lcm_set_gpio_output(GPIO_LCD_TE_EN, 0);
	MDELAY(30);
	
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, 1);
	lcm_set_gpio_output(GPIO_LCD_TE_EN, 1);
	MDELAY(50);
	
	lcm_set_gpio_output(GPIO_LCD_RST_EN, 1);
	MDELAY(50);
	
	init_lcm_registers();
	//MDELAY(180);
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
#ifdef BUILD_LK
	printf("[elink][LK/LCM] %s() enter\n", __func__);
#else
	printk("[elink][Kernel/LCM] %s() enter\n", __func__);
#endif
	
	//lcm_get_gpio_infor();
	early_init_lcm();
		
	lcm_set_gpio_output(GPIO_LCD_RST_EN, 0);
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCD_TE_EN, 0);
	MDELAY(20);
	
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, 0);
		
	init_lcm_registers();	
	//push_table(lcm_deep_sleep_mode_in_setting, 
		//sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
	
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

LCM_DRIVER jd9367_wxga_ips_dsi_vdo_lcm_drv = 
{
	.name = "jd9367_wxga_ips_dsi_vdo",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
};

