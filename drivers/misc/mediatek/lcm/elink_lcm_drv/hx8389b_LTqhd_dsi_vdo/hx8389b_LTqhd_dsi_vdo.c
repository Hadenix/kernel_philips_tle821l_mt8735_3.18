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
#define FRAME_WIDTH  										(540)
#define FRAME_HEIGHT 										(960)

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

#if defined(ELINK_MR601)
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

        #if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
        #else
		params->dsi.mode   = SYNC_PULSE_VDO_MODE; //SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE; 
        #endif
	
		// DSI
		/* Command mode setting */
	     // Three lane or Four lane
		params->dsi.LANE_NUM				= LCM_TWO_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Video mode setting		
		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		
		params->dsi.vertical_sync_active				= 0x05;// 3    2
		params->dsi.vertical_backporch					= 14;// 20   1
		params->dsi.vertical_frontporch					= 12; // 1  12
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 0x16;// 50  2
		params->dsi.horizontal_backporch				= 0x38;
		params->dsi.horizontal_frontporch				= 0x18;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

	    //params->dsi.LPX=8; 

		// Bit rate calculation
		//  Every lane speed
		//params->dsi.pll_select=1;
		//params->dsi.PLL_CLOCK  = LCM_DSI_6589_PLL_CLOCK_377;
		params->dsi.PLL_CLOCK=240;
		
#if 0		
		params->dsi.pll_div1=0;		// div1=0,1,2,3;div1_real=1,2,4,4 ----0: 546Mbps  1:273Mbps
		params->dsi.pll_div2=0;		// div2=0,1,2,3;div1_real=1,2,4,4	
#if (LCM_DSI_CMD_MODE)
		params->dsi.fbk_div =9;
#else
		params->dsi.fbk_div =9;    // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	
#endif
#endif

#elif defined(ELINK_MR680)
		memset(params, 0, sizeof(LCM_PARAMS));

		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		// enable tearing-free
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;//LCM_DBI_TE_MODE_VSYNC_OR_HSYNC;//LCM_DBI_TE_MODE_DISABLED;//LCM_DBI_TE_MODE_VSYNC_ONLY;
		params->dbi.te_edge_polarity		= LCM_POLARITY_FALLING;

	#if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
	#else
		//params->dsi.mode	 = SYNC_EVENT_VDO_MODE;
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;
	#endif


		// DSI
		/* Command mode setting */
		params->dsi.LANE_NUM				= LCM_THREE_LANE;

		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq	= LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding 	= LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format		= LCM_DSI_FORMAT_RGB888;

		params->dsi.intermediat_buffer_num = 2;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

		params->dsi.word_count=540*3;	//DSI CMD mode need set these two bellow params, different to 6577

		params->dsi.vertical_active_line=960;
		params->dsi.vertical_sync_active = 9;//3
		params->dsi.vertical_backporch = 7;//13
		params->dsi.vertical_frontporch = 7;//17
		params->dsi.vertical_active_line = FRAME_HEIGHT;

		params->dsi.horizontal_sync_active = 96;//30
		params->dsi.horizontal_backporch =46;//30
		params->dsi.horizontal_frontporch =31;//30
		params->dsi.horizontal_active_pixel = FRAME_WIDTH;


		params->dsi.PLL_CLOCK=245;

		//params->dsi.pll_div1=0; 	// div1=0,1,2,3;div1_real=1,2,4,4
		//params->dsi.pll_div2=0; 	// div2=0,1,2,3;div2_real=1,2,4,4
		//params->dsi.fbk_sel=1;		 // fbk_sel=0,1,2,3;fbk_sel_real=1,2,4,4
		//params->dsi.fbk_div =21; //34;	//28	// fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)		
#elif defined(ELINK_MR698)
		memset(params, 0, sizeof(LCM_PARAMS));

		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		// enable tearing-free
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;//LCM_DBI_TE_MODE_VSYNC_OR_HSYNC;//LCM_DBI_TE_MODE_DISABLED;//LCM_DBI_TE_MODE_VSYNC_ONLY;
		params->dbi.te_edge_polarity		= LCM_POLARITY_FALLING;

	#if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
	#else
		//params->dsi.mode	 = SYNC_EVENT_VDO_MODE;
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;
	#endif


		// DSI
		/* Command mode setting */
		params->dsi.LANE_NUM				= LCM_THREE_LANE;

		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq	= LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding 	= LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format		= LCM_DSI_FORMAT_RGB888;

		params->dsi.intermediat_buffer_num = 2;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

		params->dsi.word_count=540*3;	//DSI CMD mode need set these two bellow params, different to 6577

		params->dsi.vertical_active_line=960;
		params->dsi.vertical_sync_active = 9;//3
		params->dsi.vertical_backporch = 7;//13
		params->dsi.vertical_frontporch = 7;//17
		params->dsi.vertical_active_line = FRAME_HEIGHT;

		params->dsi.horizontal_sync_active = 96;//30
		params->dsi.horizontal_backporch =46;//30
		params->dsi.horizontal_frontporch =31;//30
		params->dsi.horizontal_active_pixel = FRAME_WIDTH;


		params->dsi.PLL_CLOCK=245;

		//params->dsi.pll_div1=0; 	// div1=0,1,2,3;div1_real=1,2,4,4
		//params->dsi.pll_div2=0; 	// div2=0,1,2,3;div2_real=1,2,4,4
		//params->dsi.fbk_sel=1;		 // fbk_sel=0,1,2,3;fbk_sel_real=1,2,4,4
		//params->dsi.fbk_div =21; //34;	//28	// fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)		

#endif
}

extern void DSI_clk_HS_mode(unsigned char enter);

static void init_lcm_registers(void)
{
	
#if 1 //defined(ELINK_MR601)  //add zhou 20131126
	 unsigned int data_array[16];		
	 data_array[0]=0x00043902;//Enable external Command
	 data_array[1]=0x8983FFB9; 
	 dsi_set_cmdq(data_array, 2, 1);
	 MDELAY(1);//3000
	 
	 data_array[0]=0x00023902;//Enable external Command
	 data_array[1]=0x0000e0c6; 
	 dsi_set_cmdq(data_array, 2, 1);
	 MDELAY(1);//3000
	 
	 data_array[0]=0x00083902;//Enable external Command//3
	 data_array[1]=0x009341ba; 
	 data_array[2]=0x1810A416; 
	 dsi_set_cmdq(data_array, 3, 1);
	 MDELAY(1);//3000

	
	data_array[0]=0x00143902;
	data_array[1]=0x060000B1;
	data_array[2]=0x111059e8;
	data_array[3]=0x4139f1d1;
	data_array[4]=0x01423f3f;
	data_array[5]=0xe600f73a;	
	dsi_set_cmdq(data_array, 6, 1);
	MDELAY(5);

/*
	 data_array[0]=0x00143902;
	 data_array[1]=0x060000B1;
	 data_array[2]=0x111059e8;
	 data_array[3]=0x4139ef39;	//0x332bf272   2a22f272
	 data_array[4]=0x01423f3f; 
	 data_array[5]=0xe600f73a;	
	 dsi_set_cmdq(&data_array, 6, 1);
	 MDELAY(1);
*/
	
	 data_array[0]=0x00083902;//Enable external Command//3 
	 data_array[1]=0x780000b2; 
	 data_array[2]=0x803f070c;
	 dsi_set_cmdq(data_array, 3, 1); 
	 MDELAY(1); 
	
	
	 data_array[0]=0x00183902;//Enable external Command//3
	 data_array[1]=0x000800b4;	 //0x000480b4
	 data_array[2]=0x32041032; 
	 data_array[3]=0x10320010; 
	 data_array[4]=0x400A3700; 
	 data_array[5]=0x40023708; //0x380a1301
	 data_array[6]=0x0A585014;	
	 dsi_set_cmdq(data_array, 7, 1);
	 MDELAY(1);//3000
	 
	 data_array[0]=0x00393902;//Enable external Command//3
	 data_array[1]=0x000000d5; 
	 data_array[2]=0x00000100; 
	 data_array[3]=0x99006000; 
	 data_array[4]=0x88bbaa88; 
	 data_array[5]=0x88018823; 
	 data_array[6]=0x01458867; 
	 data_array[7]=0x88888823; 
	 data_array[8]=0x99888888; 
	 data_array[9]=0x5488aabb; 
	 data_array[10]=0x10887688; 
	 data_array[11]=0x10323288; 
	 data_array[12]=0x88888888; 
	 data_array[13]=0x00040088; 
	 data_array[14]=0x00000000; 
	 data_array[15]=0x00000000; 
	 dsi_set_cmdq(data_array, 16, 1);
	 MDELAY(1);//3000
	 
	 data_array[0]=0x00053902;//Enable external Command
	 data_array[1]=0x007500b6; 
	 data_array[2]=0x00000075; 
	 dsi_set_cmdq(data_array, 3, 1);
	MDELAY(1);//3000
	 
	 
	 data_array[0]=0x00233902;
	 data_array[1]=0x1c1a00e0; 
	 data_array[2]=0x2c3f352f; 
	 data_array[3]=0x0e0d0b44; 
	 data_array[4]=0x13111412; 
	 data_array[5]=0x1a001711; 
	 data_array[6]=0x3f352f1c; 
	 data_array[7]=0x0d0b442c; 
	 data_array[8]=0x1114120e; 
	 data_array[9]=0x00171113; 
	 dsi_set_cmdq(data_array, 10, 1);
	 MDELAY(1);//3000
	 
	 data_array[0]=0x00023902;//e
	 data_array[1]=0x000002cc;
	 dsi_set_cmdq(data_array, 2, 1);
	 MDELAY(1);
	 
	 data_array[0]=0x00043902;//e
	 data_array[1]=0x500000b7;
	 dsi_set_cmdq(data_array, 2, 1);
	 MDELAY(1);
	
	// data_array[0] = 0x00000500; //0x11,exit sleep mode,1byte
	// dsi_set_cmdq(&data_array, 1, 1);
	// MDELAY(120);//5000
	
	 data_array[0] = 0x00110500; //0x11,exit sleep mode,1byte
	 dsi_set_cmdq(data_array, 1, 1);
	 MDELAY(120);//5000
	 
	 data_array[0] = 0x00290500; //0x11,exit sleep mode,1byte
	 dsi_set_cmdq(data_array, 1, 1);
	 MDELAY(30);//5000
			 
#elif defined(ELINK_MR680)	//add zhou 20131126
	unsigned int data_array[16];
	data_array[0]=0x00043902;
	data_array[1]=0x8983FFB9; 
	dsi_set_cmdq(data_array, 2, 1); 
	MDELAY(10);

	data_array[0]=0x00083902;
	data_array[1]=0x009341BA; //06->04
	data_array[2]=0x1810A416; 
	dsi_set_cmdq(data_array, 3, 1);
	MDELAY(1);
	data_array[0]=0x00023902;
	data_array[1]=0x000008c6; //06->04
	dsi_set_cmdq(data_array, 2, 1); 
	MDELAY(1); 
	data_array[0]=0x00143902;
	data_array[1]=0x060000B1; //06->04 05
	data_array[2]=0x11149EF6; 
	data_array[3]=0x2b23D1d1;//F1->F4  0x2820D171;
	data_array[4]=0x01433F3F; 
	data_array[5]=0xE600F25A;  //0xE600F62A; 
	dsi_set_cmdq(data_array, 6, 1);
	MDELAY(1);
	data_array[0]=0x00043902;
	data_array[1]=0x520000B7; 
	dsi_set_cmdq(data_array, 2, 1); 
	MDELAY(1); 
	data_array[0]=0x00083902;
	data_array[1]=0x780000B2; 
	data_array[2]=0xf03F050C;  //0xC03F0F0C; 
	dsi_set_cmdq(data_array, 3, 1);
	MDELAY(1);
	data_array[0]=0x00183902;
	data_array[1]=0x0008adB4; 
	data_array[2]=0x32081032; 
	data_array[3]=0x1032CC13; 
	data_array[4]=0x400A3700; 
	data_array[5]=0x2D02370A; //0x2D02370A;//0x3c02370A;
	data_array[6]=0x0A3E3C14; //0x0A3E3C14; //0x0A5c5814;
	dsi_set_cmdq(data_array, 7, 1);
	MDELAY(1);
	data_array[0]=0x00393902;
	data_array[1]=0x000000D5; 
	data_array[2]=0x00000100; 
	data_array[3]=0x801EE001; 
	data_array[4]=0x82381882; 
	data_array[5]=0x84783886; 
	data_array[6]=0x78185880; 
	data_array[7]=0x80848658;
	data_array[8]=0x85878888; 
	data_array[9]=0x85814868; 
	data_array[10]=0x83874808; 
	data_array[11]=0x28082868; 
	data_array[12]=0x88888381; //0x88888888;
	data_array[13]=0x00040088; //0x00040088;
	data_array[14]=0x00000000; 
	data_array[15]=0x00000000; 
	dsi_set_cmdq(data_array, 16, 1);
	MDELAY(1); 
	data_array[0]=0x00033902;
	data_array[1]=0x001743c0;  
	dsi_set_cmdq(data_array, 2, 1); 
	MDELAY(1);
	data_array[0]=0x00053902;
	data_array[1]=0x008000B6;//9d 
	data_array[2]=0x00000080;//9d 
	dsi_set_cmdq(data_array, 3, 1);
	MDELAY(1);
	data_array[0]=0x00233902;
	data_array[1]=0x090a02E0; 
	data_array[2]=0x2d3B1415; 
	data_array[3]=0x13110E39; 
	data_array[4]=0x16151817; 
	data_array[5]=0x0A021710; 
	data_array[6]=0x3B141509; 
	data_array[7]=0x110E392d; 
	data_array[8]=0x15181713; 
	data_array[9]=0x00171016; 
	dsi_set_cmdq(data_array, 10, 1);
	MDELAY(1); 

	data_array[0]=0x00023902;
	data_array[1]=0x000002CC;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);
	data_array[0] = 0x00110500; 
	dsi_set_cmdq(data_array, 1, 1); 
	MDELAY(10);
	data_array[0]=0x00053902;
	data_array[1]=0x125805DE;
	data_array[2]=0x00000006; 
	dsi_set_cmdq(data_array, 3, 1);
	MDELAY(1); 
	data_array[0] = 0x00290500; 
	dsi_set_cmdq(data_array, 1, 1); 
	MDELAY(100);
#elif defined(ELINK_MR698)	//add zhou 20131126
      unsigned int data_array[35];

#if 1       
	 data_array[0]=0x00043902;//Enable external Command
	 data_array[1]=0x8983FFB9; 
	 dsi_set_cmdq(data_array, 2, 1);
	 MDELAY(1);//3000
	 
	 data_array[0]=0x00023902;//Enable external Command
	 data_array[1]=0x000008c6; 
	 dsi_set_cmdq(data_array, 2, 1);
	 MDELAY(1);//3000
	 
	 data_array[0]=0x00033902;//Enable external Command//3
	 data_array[1]=0x009341ba; 
	 //data_array[2]=0x1810A416; 
	 dsi_set_cmdq(data_array, 2, 1);
	 MDELAY(1);//3000

	
	data_array[0]=0x00143902;
	data_array[1]=0x070000B1;
	data_array[2]=0x111098ee;
	data_array[3]=0x3129f595;
	data_array[4]=0x01433f3f;
	data_array[5]=0xe600f25a;	
	dsi_set_cmdq(data_array, 6, 1);
	MDELAY(5);
	
	 data_array[0]=0x00083902;//Enable external Command//3 
	 data_array[1]=0x780000b2; 
	 data_array[2]=0x803f070c;
	 dsi_set_cmdq(data_array, 3, 1); 
	 MDELAY(1); 
	
	
	 data_array[0]=0x00183902;//Enable external Command//3
	 data_array[1]=0x000880b4;	 //0x000480b4
	 data_array[2]=0x32041032; 
	 data_array[3]=0x10320010; 
	 data_array[4]=0x400A3700; 
	 data_array[5]=0x46003708; //0x380a1301
	 data_array[6]=0x02ffff06;	
	 dsi_set_cmdq(data_array, 7, 1);
	 MDELAY(1);//3000

	 data_array[0]=0x00033902;//Enable external Command//3 
	 data_array[1]=0x001743c0; 
	 dsi_set_cmdq(data_array, 2, 1); 
	 MDELAY(1); 
	 data_array[0]=0x00393902;//Enable external Command//3
	 data_array[1]=0x000000d5; 
	 data_array[2]=0x00000100; 
	 data_array[3]=0x99006000; 
	 data_array[4]=0x88bbaa88; 
	 data_array[5]=0x88018823; 
	 data_array[6]=0x01458867; 
	 data_array[7]=0x88888823; 
	 data_array[8]=0x99888888; 
	 data_array[9]=0x5488aabb; 
	 data_array[10]=0x10887688; 
	 data_array[11]=0x10323288; 
	 data_array[12]=0x88888888; 
	 data_array[13]=0x00040088; 
	 data_array[14]=0x00000000; 
	 data_array[15]=0x00000000; 
	 dsi_set_cmdq(data_array, 16, 1);
	 MDELAY(1);//3000
	 
	 data_array[0]=0x00053902;//Enable external Command
	 data_array[1]=0x007b00b6; 
	 data_array[2]=0x0000007b; 
	 dsi_set_cmdq(data_array, 3, 1);
	MDELAY(10);//3000
	 
	 
	 data_array[0]=0x00233902;
	 data_array[1]=0x000000e0; 
	 data_array[2]=0x1e3f0e0e; 
	 data_array[3]=0x12050532; 
	 data_array[4]=0x16161816; 
	 data_array[5]=0x00001610; 
	 data_array[6]=0x3f0e0e00; 
	 data_array[7]=0x0505321e; 
	 data_array[8]=0x16181610; 
	 data_array[9]=0x00161016; 
	 dsi_set_cmdq(data_array, 10, 1);
	 MDELAY(1);//3000
/**
	data_array[0]=0x00803902;
	data_array[1]=0x040001C1;
	data_array[2]=0x2E271E12; 
	data_array[3]=0x48413933; 
	data_array[4]=0x68605850; 
	data_array[5]=0x87807870; 
	data_array[6]=0xA79F978F; 
	data_array[7]=0xC6BFB7AF; 
	data_array[8]=0xE5DED5CD; 
	data_array[9]=0x2FFFF7EF;
	data_array[10]=0x01A8A7EA;
	data_array[11]=0xC0E26B59; 
	
	data_array[12]=0x1E120400;
	data_array[13]=0x39332E27; 
	data_array[14]=0x58504841; 
	data_array[15]=0x78706860; 
	data_array[16]=0x978F8780; 
	data_array[17]=0xB7AFA79F; 
	data_array[18]=0xD5CDC6BF; 
	data_array[19]=0xF7EFE5DE; 
	data_array[20]=0xA7EA2FFF;
	data_array[21]=0x6B5901A8;
	data_array[22]=0x0400C0E2;

	data_array[23]=0x2E271E12; 
	data_array[24]=0x48413933; 
	data_array[25]=0x68605850; 
	data_array[26]=0x87807870; 
	data_array[27]=0xA79F978F; 
	data_array[28]=0xC6BFB7AF; 
	data_array[29]=0xE5DED5CD; 
	data_array[30]=0x2FFFF7EF;
	data_array[31]=0x01A8A7EA;
	data_array[32]=0xC0E26B59; 
	dsi_set_cmdq(&data_array, 33, 1);
	MDELAY(1);
*/
	 data_array[0]=0x00023902;//e
	 data_array[1]=0x000002cc;
	 dsi_set_cmdq(data_array, 2, 1);
	 MDELAY(1);
	 
	 data_array[0]=0x00043902;//e
	 data_array[1]=0xd00500b7;
	 dsi_set_cmdq(data_array, 2, 1);
	 MDELAY(1);
	
	 data_array[0] = 0x00110500; //0x11,exit sleep mode,1byte
	 dsi_set_cmdq(data_array, 1, 1);
	 MDELAY(120);//5000
	 
	 data_array[0] = 0x00290500; //0x11,exit sleep mode,1byte
	 dsi_set_cmdq(data_array, 1, 1);
	 MDELAY(30);//5000

#else
	data_array[0]=0x00043902;
	data_array[1]=0x8983FFB9; //06->04
	dsi_set_cmdq(&data_array, 2, 1);
	MDELAY(1);
	data_array[0]=0x08C61500;
	dsi_set_cmdq(data_array, 1, 1); 

	data_array[0]=0x00033902;
	data_array[1]=0x009341BA; 
	dsi_set_cmdq(data_array, 2, 1); 


	

	data_array[0]=0x0014902;
	data_array[1]=0x070000B1; //06->04 05
	data_array[2]=0x111098EE; 
	data_array[3]=0x3129F595;//F1->F4  0x2820D171;
	data_array[4]=0x01433F3F; 
	data_array[5]=0xE600F25A;  //0xE600F62A; 
	dsi_set_cmdq(data_array, 6, 1);
	MDELAY(10);
	

	data_array[0]=0x00083902;
	data_array[1]=0x780000B2; 
	data_array[2]=0x803F070C; 
	dsi_set_cmdq(data_array, 3, 1);
	MDELAY(1);


	
	data_array[0]=0x00183902;
	data_array[1]=0x000880B4; 
	data_array[2]=0x32041032; 
	data_array[3]=0x10320010; 
	data_array[4]=0x400A3700; 
	data_array[5]=0x46003708; 
	data_array[6]=0x02FFFF06; 
	dsi_set_cmdq(data_array, 7, 1);	
	MDELAY(10); 

	
	data_array[0]=0x00033902;
	data_array[1]=0X001743C0;  
	dsi_set_cmdq(data_array, 2, 1); 
	MDELAY(10);

	
	
	data_array[0]=0x00393902;
	data_array[1]=0x000000D5; 
	data_array[2]=0x00000100; 
	data_array[3]=0x99006000; 
	data_array[4]=0x88BBAA88; 
	data_array[5]=0x88018823; 
	data_array[6]=0x01458867; 
	data_array[7]=0x88888823; 
	data_array[8]=0x99888888; 
	data_array[9]=0x5488AABB;
	data_array[10]=0x10887688; 
	data_array[11]=0x10323288;
	data_array[12]=0x88888888; 
	data_array[13]=0x00040088; 
	data_array[14]=0x00000000; 
	data_array[15]=0x00000000; 
	dsi_set_cmdq(data_array, 16, 1);
	MDELAY(1); 



	data_array[0]=0x00053902;
	data_array[1]=0x000700B6;
	data_array[2]=0x0000007B;
	dsi_set_cmdq(data_array, 3, 1);

	MDELAY(10);	
	data_array[0]=0x00233902;
	data_array[1]=0x000000E0; 
	data_array[2]=0x1E3F0E0E; 
	data_array[3]=0x12050532; 
	data_array[4]=0x16161816; 
	data_array[5]=0x00001610; 
	data_array[6]=0x3F0E0E00; 
	data_array[7]=0x0505321E; 
	data_array[8]=0x16181610; 
	data_array[9]=0x00161016;
	dsi_set_cmdq(data_array, 10, 1);
	MDELAY(10);


	data_array[0]=0x00843902;
	data_array[1]=0x040001C1;
	data_array[2]=0x2E271E12; 
	data_array[3]=0x48413933; 
	data_array[4]=0x68605850; 
	data_array[5]=0x87807870; 
	data_array[6]=0xA79F978F; 
	data_array[7]=0xC6BFB7AF; 
	data_array[8]=0xE5DED5CD; 
	data_array[9]=0x2FFFF7EF;
	data_array[10]=0x01A8A7EA;
	data_array[11]=0xC0E26B59; 
	
	data_array[12]=0x1E120400;
	data_array[13]=0x39332E27; 
	data_array[14]=0x58504841; 
	data_array[15]=0x78706860; 
	data_array[16]=0x978F8780; 
	data_array[17]=0xB7AFA79F; 
	data_array[18]=0xD5CDC6BF; 
	data_array[19]=0xF7EFE5DE; 
	data_array[20]=0xA7EA2FFF;
	data_array[21]=0x6B5901A8;
	data_array[22]=0x0400C0E2;

	data_array[23]=0x2E271E12; 
	data_array[24]=0x48413933; 
	data_array[25]=0x68605850; 
	data_array[26]=0x87807870; 
	data_array[27]=0xA79F978F; 
	data_array[28]=0xC6BFB7AF; 
	data_array[29]=0xE5DED5CD; 
	data_array[30]=0x2FFFF7EF;
	data_array[31]=0x01A8A7EA;
	data_array[32]=0xC0E26B59; 
	dsi_set_cmdq(data_array, 33, 1);
	MDELAY(1);
	
	//data_array[0]= 0x02CC1500; 
	//dsi_set_cmdq(&data_array, 1, 1); 
	data_array[0]=0x00023902;
	data_array[1]=0x000002CC;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0]=0x00033902;
	data_array[1]=0x000500B7;
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00110500; 
	dsi_set_cmdq(data_array, 1, 1); 
	MDELAY(10);

	MDELAY(1); 
	data_array[0] = 0x00290500; 
	dsi_set_cmdq(data_array, 1, 1); 
	MDELAY(100);
	
	#endif
#else
	data_array[0]=0x00043902;
	data_array[1]=0x8983FFB9; 
	dsi_set_cmdq(data_array, 2, 1); 
	MDELAY(10);
	 
	data_array[0]=0x00083902;
	data_array[1]=0x009341BA; //06->04
	data_array[2]=0x1810A416; 
	dsi_set_cmdq(data_array, 3, 1);
	MDELAY(1);
	data_array[0]=0x00023902;
	data_array[1]=0x000008c6; //06->04
	dsi_set_cmdq(data_array, 2, 1); 
	MDELAY(1); 
	data_array[0]=0x00143902;
	data_array[1]=0x060000B1; //06->04 05
	data_array[2]=0x11149EF6; 
	data_array[3]=0x2b23D1d1;//F1->F4  0x2820D171;
	data_array[4]=0x01433F3F; 
	data_array[5]=0xE600F25A;  //0xE600F62A; 
	dsi_set_cmdq(data_array, 6, 1);
	MDELAY(1);
	data_array[0]=0x00043902;
	data_array[1]=0x520000B7; 
	dsi_set_cmdq(data_array, 2, 1); 
	MDELAY(1); 
	data_array[0]=0x00083902;
	data_array[1]=0x780000B2; 
	data_array[2]=0xf03F050C;  //0xC03F0F0C; 
	dsi_set_cmdq(data_array, 3, 1);
	MDELAY(1);
	data_array[0]=0x00183902;
	data_array[1]=0x0008ADB4; 
	data_array[2]=0x32081032; 
	data_array[3]=0x1032CC13; 
	data_array[4]=0x400A3700; 
	data_array[5]=0x3c02370A; //0x2D02370A;
	data_array[6]=0x0A5c5814; //0x0A3E3C14; 
	dsi_set_cmdq(data_array, 7, 1);
	MDELAY(1);
	data_array[0]=0x00363902;
	data_array[1]=0x000000D5; 
	data_array[2]=0x00000100; 
	data_array[3]=0x801EE001; 
	data_array[4]=0x82381882; 
	data_array[5]=0x84783886; 
	data_array[6]=0x78185880; 
	data_array[7]=0x80848658;
	data_array[8]=0x85878888; 
	data_array[9]=0x85814868; 
	data_array[10]=0x83874808; 
	data_array[11]=0x28082868; 
	data_array[12]=0x88888381; //0x88888888;
	data_array[13]=0x00000088; //0x00040088;
	data_array[14]=0x00000000; 
	//data_array[15]=0x00000000; 
	dsi_set_cmdq(data_array, 15, 1);
	MDELAY(1); 
	data_array[0]=0x00033902;
	data_array[1]=0x001743c0;  
	dsi_set_cmdq(data_array, 2, 1); 
	MDELAY(1);
	data_array[0]=0x00053902;

	data_array[1]=0x00|(i<<16)|00|0xB6; 
	data_array[2]=0x00|00|00|i;
	i+=2;

	
	//data_array[1]=0x007b00B6; 
	//data_array[2]=0x0000007b; 
	dsi_set_cmdq(data_array, 3, 1);
	MDELAY(1);
	data_array[0]=0x00233902;
	data_array[1]=0x090a02E0; 
	data_array[2]=0x2d3B1415; 
	data_array[3]=0x13110E39; 
	data_array[4]=0x16151817; 
	data_array[5]=0x0A021710; 
	data_array[6]=0x3B141509; 
	data_array[7]=0x110E392d; 
	data_array[8]=0x15181713; 
	data_array[9]=0x00171016; 
	dsi_set_cmdq(data_array, 10, 1);
	MDELAY(1); 
	 
	data_array[0]=0x00023902;
	data_array[1]=0x000002CC;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(1);

	data_array[0] = 0x00110500; 
	dsi_set_cmdq(data_array, 1, 1); 
	MDELAY(200);
	data_array[0]=0x00053902;
	data_array[1]=0x125805DE;
	data_array[2]=0x00000006; 
	dsi_set_cmdq(data_array, 3, 1);
	MDELAY(1); 
	data_array[0] = 0x00290500; 
	dsi_set_cmdq(data_array, 1, 1); 
	MDELAY(50);

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

LCM_DRIVER hx8389b_LTqhd_dsi_vdo_lcm_drv = 
{
    .name			= "hx8389b_LTqhd_dsi_vdo",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
#if (LCM_DSI_CMD_MODE)
	.set_backlight	= lcm_setbacklight,
    .update         = lcm_update,
#endif
   
};

