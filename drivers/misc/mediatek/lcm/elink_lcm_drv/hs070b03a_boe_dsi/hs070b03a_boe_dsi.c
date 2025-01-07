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

	params->physical_height = 173;
	params->physical_width 	= 108;
	
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
	params->dsi.LANE_NUM				= LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

	// Highly depends on LCD driver capability.
	// Not support in MT6573
	params->dsi.packet_size=256;

	// Video mode setting		
	params->dsi.intermediat_buffer_num = 2; //because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	
	params->dsi.word_count=FRAME_WIDTH*3; 

	

	params->dsi.vertical_sync_active = 2; 
	params->dsi.vertical_backporch =  15; 
	params->dsi.vertical_frontporch = 3; 
	params->dsi.vertical_active_line = FRAME_HEIGHT;

	params->dsi.horizontal_sync_active = 4; 
	params->dsi.horizontal_backporch =	16; 
	params->dsi.horizontal_frontporch = 10; 
	params->dsi.horizontal_active_pixel = FRAME_WIDTH;

	params->dsi.PLL_CLOCK=200; //
}

extern void DSI_clk_HS_mode(unsigned char enter);
#if 1
static void init_lcm_registers(void)
{
	unsigned int data_array[16];
	
#ifdef BUILD_LK
		printf("[Elink_LCM_Drv Nicholas OTM1283A] %s, LK \n", __func__);
#else
		printk("[Elink_LCM_Drv Nicholas OTM1283A] %s, kernel", __func__);
#endif

	
         data_array[0] = 0x00E01500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x93E11500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x65E21500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0xF8E31500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x04E01500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x691E1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x8B2B1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x00E01500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x03801500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x01E01500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x00001500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x9F011500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x00031500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x9F041500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x00171500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0xB1181500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x00191500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x001A1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0xB11B1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x001C1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x3E1F1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x2D201500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x2D211500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x0E221500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x38241500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0xF1261500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x44271500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x19371500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x05381500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x0C391500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x183A1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x06401500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0xA0411500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x0F551500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x01561500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x69571500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x0A581500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x0A591500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x7C5D1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x6D5E1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x615F1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x55601500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x53611500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x44621500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x48631500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x31641500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x49651500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x47661500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x47671500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x65681500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x50691500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x5C6A1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x4F6B1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x4F6C1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x3E6D1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x366E1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x006F1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x7C701500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x6D711500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x61721500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x55731500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x53741500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x44751500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x48761500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x31771500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x49781500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x47791500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x477A1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x657B1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x507C1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x5C7D1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x4F7E1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x4F7F1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x3E801500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x36811500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x00821500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x02E01500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x05001500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x05011500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x04021500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x04031500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x07041500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x07051500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x06061500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x06071500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x00081500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F091500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F0A1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F0B1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F0C1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F0D1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F0E1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x010F1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F101500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F111500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F121500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F131500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F141500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F151500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x05161500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x05171500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x04181500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x04191500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x071A1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x071B1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x061C1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x061D1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x001E1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F1F1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F201500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F211500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F221500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F231500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F241500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x01251500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F261500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F271500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F281500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F291500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F2A1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F2B1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x10581500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x00591500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x005A1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x105B1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x0E5C1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x205D1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x005E1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x005F1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x00601500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x00611500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x00621500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x03631500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x70641500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x00651500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x00661500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x31671500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x10681500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x06691500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x706A1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x006B1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x006C1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x046D1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x046E1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x886F1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x00701500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x00711500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x06721500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x7B731500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x00741500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x87751500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x00761500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x5D771500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x17781500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x1F791500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x007A1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x007B1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x007C1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x037D1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x7B7E1500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x03E01500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x3E981500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x00E01500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x02E61500;
         dsi_set_cmdq(data_array, 1, 1);
         data_array[0] = 0x02E71500;
         dsi_set_cmdq(data_array, 1, 1);	
	
		data_array[0] = 0x00110500; // Sleep In
		dsi_set_cmdq( data_array, 1, 1);
		MDELAY(150);  
		data_array[0] = 0x00290500;
		dsi_set_cmdq( data_array, 1, 1);
		MDELAY(50);

		data_array[0] = 0x05E01500;
		dsi_set_cmdq(data_array, 1, 1);			
		data_array[0] = 0x16001500;
		dsi_set_cmdq(data_array, 1, 1);
		data_array[0] = 0x28011500;
		dsi_set_cmdq(data_array, 1, 1);         
		data_array[0] = 0x00E01500;
		dsi_set_cmdq(data_array, 1, 1); 

#ifdef BUILD_LK
	printf("[elink][LK/LCM] %s() end\n", __func__);
#else
	printk("[elink][Kernel/LCM] %s() end\n", __func__);
#endif
}
#endif

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

LCM_DRIVER hs070b03a_boe_dsi_lcm_drv = 
{
    .name			= "hs070b03a_boe_dsi",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	//.compare_id     = lcm_compare_id,
#if (LCM_DSI_CMD_MODE)
	/*.set_backlight	= lcm_setbacklight,*/
	/* .set_pwm        = lcm_setpwm, */
	/* .get_pwm        = lcm_getpwm, */
	/*.update         = lcm_update, */
#endif
   
};

