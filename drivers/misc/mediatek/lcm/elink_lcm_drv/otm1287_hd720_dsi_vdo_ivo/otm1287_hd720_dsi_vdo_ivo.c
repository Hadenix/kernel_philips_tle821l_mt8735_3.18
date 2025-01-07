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
#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1280)

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

	
	// 5.94?? 
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


// ??¨¬?¨º¡é¨º¨¤ 720*1280 ,  TM 2¡ê¨¢¡ì,OTM1287
	
//	data_array[0]=0x00023902;
	data_array[0] = 0x00001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x00043902;
	data_array[1]=0x018712FF;	
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0] = 0x80001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x00033902;
	data_array[1]=0x008712FF;   
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0xA6001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0] = 0x0FB31500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0x80001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x000A3902;
	data_array[1]=0x006400C0;  
	data_array[2]=0x64001010;
	data_array[3]=0x00001010;
	dsi_set_cmdq(data_array, 4, 1);
	MDELAY(1);//3000
	
	
//	data_array[0]=0x00023902;
	data_array[0]=0x90001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x00073902;
	data_array[1]=0x005C00C0;  
	data_array[2]=0x00040001;
	dsi_set_cmdq(data_array, 3, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0xB3001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x00033902;
	data_array[1]=0x005500C0;  
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0x81001500;   
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0] = 0x66C11500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0xA0001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x000F3902;
	data_array[1]=0x041005C4;
	data_array[2]=0x1A150502;
	data_array[3]=0x02051005;
	data_array[4]=0x001A1505;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0xB0001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x00033902;
	data_array[1]=0x000000C4;  
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);//3000
	
	
//	data_array[0]=0x00023902;
	data_array[0]=0x91001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	
	data_array[0]=0x00033902;
	data_array[1]=0x00D247C5;  
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);//3000
	
	
//	data_array[0]=0x00023902;
	data_array[0]=0x00001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x00033902;
	data_array[1]=0x00C7C7D8; 
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);//3000
	
	
//	data_array[0]=0x00023902;
	data_array[0]=0x00001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0] = 0x3CD91500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0xB3001500;            //VDD_18V=1.7V, LVDSVDD=1.6V
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0] = 0x84C51500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00063902;
	data_array[0]=0xBB001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0] = 0x8AC51500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	
//	data_array[0]=0x00023902;
	data_array[0] = 0x82001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0] = 0x0AC41500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0] = 0xC6001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0] = 0x03B01500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	
//	data_array[0]=0x00023902;
	data_array[0] = 0x00001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0] = 0x40D01500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0] = 0xB0001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x00033902;
		data_array[1]=0x000000D1;  
		dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);//3000
	
	
//	data_array[0]=0x00023902;
	data_array[0]=0x80001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x000C3902;
	data_array[1]=0x000000CB;  
	data_array[2]=0x00000000;
	data_array[3]=0x00000000;
	dsi_set_cmdq(data_array, 4, 1);
	MDELAY(1);//3000

	data_array[0] = 0x90001500;
//	data_array[1]=0X00009000;  //0x00,0x90;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x00103902;
	data_array[1]=0x000000CB;  
	data_array[2]=0x00000000;
	data_array[3]=0x00000000;
	data_array[4]=0x00000000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0xA0001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x00103902;
	data_array[1]=0x000000CB;  
	data_array[2]=0x00000000;
	data_array[3]=0x00000000;
	data_array[4]=0x00000000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0] = 0xB0001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x00103902;
	data_array[1]=0x000000CB;  
	data_array[2]=0x00000000;
	data_array[3]=0x00000000;
	data_array[4]=0x00000000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0xC0001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x00103902;
	data_array[1]=0x050505CB;  
	data_array[2]=0x00050505;
	data_array[3]=0x00000000;
	data_array[4]=0x00000000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0xD0001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	
	data_array[0]=0x00103902;
	data_array[1]=0x000000CB;  
	data_array[2]=0x00000505;
	data_array[3]=0x05050505;
	data_array[4]=0x00000505;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0xE0001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x000F3902;
	data_array[1]=0x000000CB;  
	data_array[2]=0x00000000;
	data_array[3]=0x05000000;
	data_array[4]=0x00000005;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0xF0001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x000C3902;
	data_array[1]=0xFFFFFFCB;  
	data_array[2]=0xFFFFFFFF;
	data_array[3]=0xFFFFFFFF;
	dsi_set_cmdq(data_array, 4, 1);
	MDELAY(1);//3000
	
	
//	data_array[0]=0x00023902;
	data_array[0]=0x80001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	
	data_array[0]=0x00103902;
	data_array[1]=0x0F0507CC;  
	data_array[2]=0x00090B0D;
	data_array[3]=0x00000000;
	data_array[4]=0x00000000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00063902;
	data_array[0]=0x90001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	
	data_array[0]=0x00103902;
	data_array[1]=0x000000CC;  
	data_array[2]=0x00000301;
	data_array[3]=0x0E100608;
	data_array[4]=0x00000A0C;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);//3000
	
	
//	data_array[0]=0x00023902;
	data_array[0]=0xA0001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	
	data_array[0]=0x000F3902;
	data_array[1]=0x000000CC;  
	data_array[2]=0x00000000;
	data_array[3]=0x02000000;
	data_array[4]=0x00000004;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0xB0001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x00103902;
	data_array[1]=0x0E0402CC;  
	data_array[2]=0x000C0A10;
	data_array[3]=0x00000000;
	data_array[4]=0x00000000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0xC0001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x00103902;
	data_array[1]=0x000000CC;  
	data_array[2]=0x00000608;
	data_array[3]=0x0F0D0301;
	data_array[4]=0x00000B09;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0xD0001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x000F3902;
	data_array[1]=0x000000CC; 
	data_array[2]=0x00000000;
	data_array[3]=0x07000000;
	data_array[4]=0x00000005;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0x80001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x000D3902;
	data_array[1]=0x100387CE;  
	data_array[2]=0x85100386;
	data_array[3]=0x03841003;
	data_array[4]=0x00000010;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0x90001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x000D3902;
	data_array[1]=0x10FB34CE;  
	data_array[2]=0x3410FC34;
	data_array[3]=0xFE3410FD;
	data_array[4]=0x00000010;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0xA0001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x000F3902;
	data_array[1]=0x040538CE;  
	data_array[2]=0x001000FD;
	data_array[3]=0xFE040438;
	data_array[4]=0x00001000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0xB0001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x000F3902;
	data_array[1]=0x040338CE;  
	data_array[2]=0x001000FF;
	data_array[3]=0x00050238;
	data_array[4]=0x00001000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0] = 0xC0001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x000F3902;
	data_array[1]=0x050138CE;  
	data_array[2]=0x00100001;
	data_array[3]=0x02050038;
	data_array[4]=0x00001000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00063902;
	data_array[0]=0xD0001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x000F3902;
	data_array[1]=0x050030CE;  
	data_array[2]=0x00100003;
	data_array[3]=0x04050130;
	data_array[4]=0x00001000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);//3000
	
	
//	data_array[0]=0x00023902;
	data_array[0]=0x80001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x000F3902;
	data_array[1]=0x050230CF;  
	data_array[2]=0x00100005;
	data_array[3]=0x06050330;
	data_array[4]=0x00001000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0x90001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	
	data_array[0]=0x000F3902;
	data_array[1]=0x050430CF;  
	data_array[2]=0x00100007;
	data_array[3]=0x08050530;
	data_array[4]=0x00001000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0xA0001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x000F3902;
	data_array[1]=0x000000CF;  
	data_array[2]=0x00000000;
	data_array[3]=0x00000000;
	data_array[4]=0x00000000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0xB0001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x000F3902;
	data_array[1]=0x000000CF;  
	data_array[2]=0x00000000;
	data_array[3]=0x00000000;
	data_array[4]=0x00000000;
	dsi_set_cmdq(data_array, 5, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0xC0001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x000C3902;
	data_array[1]=0x200101CF;  
	data_array[2]=0x01000020;
	data_array[3]=0x10100182;
	dsi_set_cmdq(data_array, 4, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0xB5001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x00073902;
	data_array[1]=0xFF010CC5;  
	data_array[2]=0x00FF010C;
	dsi_set_cmdq(data_array, 3, 1);
	MDELAY(1);//3000
	
	//-------------------- for Power IC ---------------------------------
//	data_array[0]=0x00023902;
	data_array[0]=0x90001500;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x00053902;
	data_array[1]=0x021102F5;  
	data_array[2]=0x00000015;
	dsi_set_cmdq(data_array, 3, 1);
	MDELAY(1);//3000
	
	data_array[0] = 0x90001500;
//	data_array[1]=0X00009000;  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0] = 0x50C51500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0] = 0x94001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0] = 0x66C51500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	//-------------------- power on setting --------------------//
//	data_array[0]=0x00023902;
	data_array[0] = 0xB2001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x00033902;
	data_array[1]=0x000000F5;  
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0xB6001500; 
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x00033902;
	data_array[1]=0x000000F5;  
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);//3000
	
	
//	data_array[0]=0x00023902;
	data_array[0]=0x94001500;              //VCL pump dis
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x00033902;
	data_array[1]=0x000000F5;  
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0xD2001500;               //VCL reg. en
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x00033902;
	data_array[1]=0x001506F5;  
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0] = 0xB4001500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0] = 0xCCC51500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
//	data_array[0]=0x00023902;
	data_array[0]=0x00001500; 
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);//3000
	
	data_array[0]=0x00043902;
	data_array[1]=0xFFFFFFFF; 
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);//3000
	
	data_array[0] = 0x00110500; 
	dsi_set_cmdq(data_array, 1, 1); 
	MDELAY(120);

	data_array[0] = 0x00290500; 
	dsi_set_cmdq(data_array, 1, 1); 
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



LCM_DRIVER otm1287_hd720_dsi_vdo_ivo_lcm_drv = 
{
    .name			= "otm1287_hd720_dsi_vdo_ivo",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	//.compare_id     = lcm_compare_id,
};

