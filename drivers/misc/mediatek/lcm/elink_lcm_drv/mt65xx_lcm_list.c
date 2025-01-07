#include <lcm_drv.h>
#ifdef BUILD_LK
#include <platform/disp_drv_platform.h>
#else
#include <linux/delay.h>
/* #include <mach/mt_gpio.h> */
#endif

/* used to identify float ID PIN status */
#define LCD_HW_ID_STATUS_LOW      0
#define LCD_HW_ID_STATUS_HIGH     1
#define LCD_HW_ID_STATUS_FLOAT 0x02
#define LCD_HW_ID_STATUS_ERROR  0x03

#ifdef BUILD_LK
#define LCD_DEBUG(fmt)  dprintf(CRITICAL, fmt)
#else
#define LCD_DEBUG(fmt, args...)  pr_debug("[KERNEL/LCM]"fmt, ##args)
#endif

#if defined(MTK_LCM_DEVICE_TREE_SUPPORT)
	extern LCM_DRIVER lcm_common_drv;
#else
	extern LCM_DRIVER tf070mc124_dsi_lcm_drv;
	extern LCM_DRIVER tf070mc124_ips_dsi_lcm_drv;
	extern LCM_DRIVER otm1284_hd720_dsi_vdo_tm_lcm_drv;
	extern LCM_DRIVER otm1287_hd720_dsi_vdo_ivo_lcm_drv;
	extern LCM_DRIVER hx8394d_hd720_dsi_vdo_lcm_drv;
	extern LCM_DRIVER s6d7aa0x01_boe_wxga_dsi_vdo_lcm_drv;
	extern LCM_DRIVER hx8260_boe_wxga_dsi_vdo_lcm_drv;
	extern LCM_DRIVER hx8389c_hsd_qhd_dsi_vdo_lcm_drv;
	extern LCM_DRIVER hx8389c_ivo_qhd_dsi_vdo_lcm_drv;
	extern LCM_DRIVER otm1283_hd720_dsi_vdo_tm_lcm_drv;
	extern LCM_DRIVER hx8394d_hd720_dsi_cmd_lcm_drv;
	extern LCM_DRIVER kd079d1_35na_a1_lcm_drv;
	extern LCM_DRIVER mr798_hx8394c_boe_ips_dis_vdo_lcm_drv;
	extern LCM_DRIVER hx8667g_hd720_dsi_vdo_lcm_drv;
	extern LCM_DRIVER samsung_08s_dsi_vdo_lcm_drv;
	extern LCM_DRIVER jd9366_boe_ips_dsi_lcm_drv;
	extern LCM_DRIVER b116xan03_dsi_vdo_lcm_drv;
	extern LCM_DRIVER otm9605_qhd_dsi_vdo_tm_lcm_drv;
	extern LCM_DRIVER hx8389c_ivo_qhd_dsi_vdo_sl_lcm_drv;
	extern LCM_DRIVER hc101_hd720_dsi_vdo_lcm_drv;
	extern LCM_DRIVER otm1284_hd720_dsi_vdo_tm_lcm_drv;
	extern LCM_DRIVER ili6136s_wxga_boe_dsi_vdo_lcm_drv;
	extern LCM_DRIVER hs070b03a_boe_dsi_lcm_drv;
	extern LCM_DRIVER hx8389b_qhd_dsi_vdo_lcm_drv;
	extern LCM_DRIVER hx8389b_qhd_dsi_vdo_tianma_lcm_drv;
	extern LCM_DRIVER hx8389b_qhd_dsi_vdo_tianma055xdhp_lcm_drv;
	extern LCM_DRIVER hx8389b_qhd_lt_dsi_vdo_lcm_drv;
	extern LCM_DRIVER hx8389b_LTqhd_dsi_vdo_lcm_drv;
	extern LCM_DRIVER hx101_dsi_vdo_lcm_drv;
	extern LCM_DRIVER hx101_wxga_dsi_vdo_lcm_drv;
	extern LCM_DRIVER jd9366_cu800_ips_dsi_lcm_drv;
	extern LCM_DRIVER nt35521_wxga_dsi_vdo_lcm_drv;
	extern LCM_DRIVER hx8394d_boe_ips_dis_vdo_lcm_drv;
	extern LCM_DRIVER jd9367_wxga_ips_dsi_vdo_lcm_drv;
	extern LCM_DRIVER jd9362_wxga_dsi_vdo_lcm_drv;
	extern LCM_DRIVER jd9366_ivo_ips_dsi_lcm_drv;
	extern LCM_DRIVER hx8394a_ips_qc_wxga_dsi_vdo_lcm_drv;
	extern LCM_DRIVER hx8394d_wxga_ips_dsi_vdo_lcm_drv;
	extern LCM_DRIVER ek79029_ips_cpt_wxga_fy80_lcm_drv;
	extern LCM_DRIVER rm68200_auo_wxga_cc80xy_lcm_drv;
	extern LCM_DRIVER hx8394d_wxga_dsi_vdo_lcm_drv;
	extern LCM_DRIVER hx8394c_boe_wxga_dsi_vdo_lcm_drv;
	extern LCM_DRIVER hx8394f_hsd_hd720_dsi_vdo_lcm_drv;
	extern LCM_DRIVER nt35521s_am080nb03b_wxga_auo_vdo_lcm_drv;	//add by heliang 20161010
	extern LCM_DRIVER jd9366_20810800210255_wxga_cpt_vdo_lcm_drv;	//add by heliang 20161011
	extern LCM_DRIVER jd9366_am080nb03a_wxga_cpt8_lcm_drv;//add by heliang 20170324
		
#endif	







LCM_DRIVER *lcm_driver_list[] = {

#if defined(MTK_LCM_DEVICE_TREE_SUPPORT)
	&lcm_common_drv,
#else// End defined(MTK_LCM_DEVICE_TREE_SUPPORT)


#if defined(TF070MC124_DSI)  //Elink Nicholas Zheng Add 
	&tf070mc124_dsi_lcm_drv,
#endif

#if defined(TF070MC124_IPS_DSI)
	&tf070mc124_ips_dsi_lcm_drv,
#endif

#if defined(OTM1284_HD720_DSI_VDO_TM)
	&otm1284_hd720_dsi_vdo_tm_lcm_drv,
#endif
	
#if defined(OTM1287_HD720_DSI_VDO_IVO)
	&otm1287_hd720_dsi_vdo_ivo_lcm_drv, 
#endif

#if defined(HX8394D_HD720_DSI_VDO) 
	&hx8394d_hd720_dsi_vdo_lcm_drv, 
#endif

#if defined(S6D7AA0X01_BOE_WXGA_DSI_VDO)
	&s6d7aa0x01_boe_wxga_dsi_vdo_lcm_drv, 
#endif
	
#if defined(HX8260_BOE_WXGA_DSI_VDO)
	&hx8260_boe_wxga_dsi_vdo_lcm_drv,
#endif
	
#if defined(HX8389C_HSD_QHD_DSI_VDO)
	&hx8389c_hsd_qhd_dsi_vdo_lcm_drv, 
#endif
	
#if defined(HX8389C_IVO_QHD_DSI_VDO)	
	&hx8389c_ivo_qhd_dsi_vdo_lcm_drv,
#endif

#if defined(OTM1283_HD720_DSI_VDO)	
	&otm1283_hd720_dsi_vdo_tm_lcm_drv, 
#endif
	
#if defined(HX8394D_HD720_DSI_CMD)	
	&hx8394d_hd720_dsi_cmd_lcm_drv, 
#endif

#if defined(KD079D1_35NA_A1)	
	&kd079d1_35na_a1_lcm_drv,  
#endif
	
#if defined(MR798_HX8394C_BOE_IPS_DIS)	
	&mr798_hx8394c_boe_ips_dis_vdo_lcm_drv, 
#endif
	
#if defined(HX8667G_HD720_DSI_VDO)	
	&hx8667g_hd720_dsi_vdo_lcm_drv,  
#endif
	
#if defined(SAMSUNG_08S_DSI_VDO)	
	&samsung_08s_dsi_vdo_lcm_drv,  
#endif
	
#if defined(JD9366_BOE_IPS_DSI)	
	&jd9366_boe_ips_dsi_lcm_drv,  
#endif
	
#if defined(B116XAN03_DSI_VDO)	
	&b116xan03_dsi_vdo_lcm_drv,  
#endif
	
#if defined(OTM9605_QHD_DSI_VDO_TM)	
	&otm9605_qhd_dsi_vdo_tm_lcm_drv,  
#endif
	
#if defined(HX8389C_IVO_QHD_DSI_VDO_SL)	
	&hx8389c_ivo_qhd_dsi_vdo_sl_lcm_drv,  
#endif
	
#if defined(HC101_HD720_DSI_VDO)	
	&hc101_hd720_dsi_vdo_lcm_drv,  
#endif
	
#if defined(OTM1284_HD720_DSI_VDO_TM)	
	&otm1284_hd720_dsi_vdo_tm_lcm_drv,  
#endif
	
#if defined(ILI6136S_WXGA_BOE_DSI_VDO)	
	&ili6136s_wxga_boe_dsi_vdo_lcm_drv,  
#endif
	
#if defined(HS070B03A_BOE_DSI)	
	&hs070b03a_boe_dsi_lcm_drv,  
#endif
	
#if defined(HX8389B_QHD_DSI_VDO)	
	&hx8389b_qhd_dsi_vdo_lcm_drv,  
#endif
	
#if defined(HX8389B_QHD_DSI_VDO_TIANMA)	
	&hx8389b_qhd_dsi_vdo_tianma_lcm_drv, 
#endif
	
#if defined(HX8389B_QHD_DSI_VDO_TIANMA055XDHP)	
	&hx8389b_qhd_dsi_vdo_tianma055xdhp_lcm_drv,  
#endif
	
#if defined(HX8389B_QHD_LT_DSI_VDO)	
	&hx8389b_qhd_lt_dsi_vdo_lcm_drv,  
#endif
	
#if defined(HX8389B_LTQHD_DSI_VDO)	
	&hx8389b_LTqhd_dsi_vdo_lcm_drv,  
#endif
	
#if defined(HX101_DSI_VDO)	
	&hx101_dsi_vdo_lcm_drv,  
#endif
	
#if defined(HX101_WXGA_DSI_VDO)	
	&hx101_wxga_dsi_vdo_lcm_drv,  
#endif
	
#if defined(JD9366_CU800_IPS_DSI)	
	&jd9366_cu800_ips_dsi_lcm_drv, 
#endif
	
#if defined(NT35521_WXGA_DSI_VDO)	
	&nt35521_wxga_dsi_vdo_lcm_drv, 
#endif
	
#if defined(HX8394D_BOE_IPS_DIS_VDO)	
	&hx8394d_boe_ips_dis_vdo_lcm_drv, 
#endif
	
#if defined(JD9367_WXGA_IPS_DSI_VDO)	
	&jd9367_wxga_ips_dsi_vdo_lcm_drv,  
#endif
	
#if defined(JD9362_WXGA_DSI_VDO)	
	&jd9362_wxga_dsi_vdo_lcm_drv,  
#endif
	
#if defined(JD9366_IVO_IPS_DSI)	
	&jd9366_ivo_ips_dsi_lcm_drv,  
#endif
	
#if defined(HX8394A_IPS_QC_WXGA_DSI_VDO)	
	&hx8394a_ips_qc_wxga_dsi_vdo_lcm_drv,  
#endif
	
#if defined(HX8394D_WXGA_IPS_DSI_VDO)	
	&hx8394d_wxga_ips_dsi_vdo_lcm_drv, 
#endif	

#if defined(EK79029_IPS_CPT_WXGA_FY80)
	&ek79029_ips_cpt_wxga_fy80_lcm_drv,
#endif

#if defined(RM68200_AUO_WXGA_CC80XY)
	&rm68200_auo_wxga_cc80xy_lcm_drv,
#endif

#if defined(HX8394D_WXGA_DSI_VDO)
	&hx8394d_wxga_dsi_vdo_lcm_drv,
#endif

#if defined(HX8394C_BOE_WXGA_DSI_VDO)
	&hx8394c_boe_wxga_dsi_vdo_lcm_drv,
#endif

#if defined(HX8394F_HSD_HD720_DSI_VDO)
	&hx8394f_hsd_hd720_dsi_vdo_lcm_drv,
#endif
#if defined(NT35521S_AM080NB03B_WXGA_AUO_VDO)
	&nt35521s_am080nb03b_wxga_auo_vdo_lcm_drv,
#endif
#if defined(JD9366_20810800210255_WXGA_CPT_VDO)
	&jd9366_20810800210255_wxga_cpt_vdo_lcm_drv,
#endif

#if defined(JD9366_AM080NB03A_WXGA_CPT8)
	&jd9366_am080nb03a_wxga_cpt8_lcm_drv,
#endif

#endif // End not defined(MTK_LCM_DEVICE_TREE_SUPPORT)
};

#if defined(MTK_LCM_DEVICE_TREE_SUPPORT)
unsigned char lcm_name_list[][128] = {
#if defined(HX8392A_DSI_CMD)
	"hx8392a_dsi_cmd",
#endif

#if defined(HX8392A_DSI_VDO)
	"hx8392a_vdo_cmd",
#endif

#if defined(HX8392A_DSI_CMD_FWVGA)
	"hx8392a_dsi_cmd_fwvga",
#endif

#if defined(OTM9608_QHD_DSI_CMD)
	"otm9608a_qhd_dsi_cmd",
#endif

#if defined(OTM9608_QHD_DSI_VDO)
	"otm9608a_qhd_dsi_vdo",
#endif
};
#endif

#define LCM_COMPILE_ASSERT(condition) LCM_COMPILE_ASSERT_X(condition, __LINE__)
#define LCM_COMPILE_ASSERT_X(condition, line) LCM_COMPILE_ASSERT_XX(condition, line)
#define LCM_COMPILE_ASSERT_XX(condition, line) char assertion_failed_at_line_##line[(condition) ? 1 : -1]

unsigned int lcm_count = sizeof(lcm_driver_list) / sizeof(LCM_DRIVER *);
LCM_COMPILE_ASSERT(0 != sizeof(lcm_driver_list) / sizeof(LCM_DRIVER *));
#if defined(NT35520_HD720_DSI_CMD_TM) | defined(NT35520_HD720_DSI_CMD_BOE) | \
	defined(NT35521_HD720_DSI_VDO_BOE) | defined(NT35521_HD720_DSI_VIDEO_TM)
static unsigned char lcd_id_pins_value = 0xFF;

/**
 * Function:       which_lcd_module_triple
 * Description:    read LCD ID PIN status,could identify three status:highlowfloat
 * Input:           none
 * Output:         none
 * Return:         LCD ID1|ID0 value
 * Others:
 */
unsigned char which_lcd_module_triple(void)
{
	unsigned char  high_read0 = 0;
	unsigned char  low_read0 = 0;
	unsigned char  high_read1 = 0;
	unsigned char  low_read1 = 0;
	unsigned char  lcd_id0 = 0;
	unsigned char  lcd_id1 = 0;
	unsigned char  lcd_id = 0;
	/*Solve Coverity scan warning : check return value*/
	unsigned int ret = 0;

	/*only recognise once*/
	if (0xFF != lcd_id_pins_value)
		return lcd_id_pins_value;

	/*Solve Coverity scan warning : check return value*/
	ret = mt_set_gpio_mode(GPIO_DISP_ID0_PIN, GPIO_MODE_00);
	if (0 != ret)
		LCD_DEBUG("ID0 mt_set_gpio_mode fail\n");

	ret = mt_set_gpio_dir(GPIO_DISP_ID0_PIN, GPIO_DIR_IN);
	if (0 != ret)
		LCD_DEBUG("ID0 mt_set_gpio_dir fail\n");

	ret = mt_set_gpio_pull_enable(GPIO_DISP_ID0_PIN, GPIO_PULL_ENABLE);
	if (0 != ret)
		LCD_DEBUG("ID0 mt_set_gpio_pull_enable fail\n");

	ret = mt_set_gpio_mode(GPIO_DISP_ID1_PIN, GPIO_MODE_00);
	if (0 != ret)
		LCD_DEBUG("ID1 mt_set_gpio_mode fail\n");

	ret = mt_set_gpio_dir(GPIO_DISP_ID1_PIN, GPIO_DIR_IN);
	if (0 != ret)
		LCD_DEBUG("ID1 mt_set_gpio_dir fail\n");

	ret = mt_set_gpio_pull_enable(GPIO_DISP_ID1_PIN, GPIO_PULL_ENABLE);
	if (0 != ret)
		LCD_DEBUG("ID1 mt_set_gpio_pull_enable fail\n");

	/*pull down ID0 ID1 PIN*/
	ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_DOWN);
	if (0 != ret)
		LCD_DEBUG("ID0 mt_set_gpio_pull_select->Down fail\n");

	ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_DOWN);
	if (0 != ret)
		LCD_DEBUG("ID1 mt_set_gpio_pull_select->Down fail\n");

	/* delay 100ms , for discharging capacitance*/
	mdelay(100);
	/* get ID0 ID1 status*/
	low_read0 = mt_get_gpio_in(GPIO_DISP_ID0_PIN);
	low_read1 = mt_get_gpio_in(GPIO_DISP_ID1_PIN);
	/* pull up ID0 ID1 PIN */
	ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_UP);
	if (0 != ret)
		LCD_DEBUG("ID0 mt_set_gpio_pull_select->UP fail\n");

	ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_UP);
	if (0 != ret)
		LCD_DEBUG("ID1 mt_set_gpio_pull_select->UP fail\n");

	/* delay 100ms , for charging capacitance */
	mdelay(100);
	/* get ID0 ID1 status */
	high_read0 = mt_get_gpio_in(GPIO_DISP_ID0_PIN);
	high_read1 = mt_get_gpio_in(GPIO_DISP_ID1_PIN);

	if (low_read0 != high_read0) {
		/*float status , pull down ID0 ,to prevent electric leakage*/
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_DOWN);
		if (0 != ret)
			LCD_DEBUG("ID0 mt_set_gpio_pull_select->Down fail\n");

		lcd_id0 = LCD_HW_ID_STATUS_FLOAT;
	} else if ((LCD_HW_ID_STATUS_LOW == low_read0) && (LCD_HW_ID_STATUS_LOW == high_read0)) {
		/*low status , pull down ID0 ,to prevent electric leakage*/
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_DOWN);
		if (0 != ret)
			LCD_DEBUG("ID0 mt_set_gpio_pull_select->Down fail\n");

		lcd_id0 = LCD_HW_ID_STATUS_LOW;
	} else if ((LCD_HW_ID_STATUS_HIGH == low_read0) && (LCD_HW_ID_STATUS_HIGH == high_read0)) {
		/*high status , pull up ID0 ,to prevent electric leakage*/
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_UP);
		if (0 != ret)
			LCD_DEBUG("ID0 mt_set_gpio_pull_select->UP fail\n");

		lcd_id0 = LCD_HW_ID_STATUS_HIGH;
	} else {
		LCD_DEBUG(" Read LCD_id0 error\n");
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_DISABLE);
		if (0 != ret)
			LCD_DEBUG("ID0 mt_set_gpio_pull_select->Disbale fail\n");

		lcd_id0 = LCD_HW_ID_STATUS_ERROR;
	}


	if (low_read1 != high_read1) {
		/*float status , pull down ID1 ,to prevent electric leakage*/
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_DOWN);
		if (0 != ret)
			LCD_DEBUG("ID1 mt_set_gpio_pull_select->Down fail\n");

		lcd_id1 = LCD_HW_ID_STATUS_FLOAT;
	} else if ((LCD_HW_ID_STATUS_LOW == low_read1) && (LCD_HW_ID_STATUS_LOW == high_read1)) {
		/*low status , pull down ID1 ,to prevent electric leakage*/
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_DOWN);
		if (0 != ret)
			LCD_DEBUG("ID1 mt_set_gpio_pull_select->Down fail\n");

		lcd_id1 = LCD_HW_ID_STATUS_LOW;
	} else if ((LCD_HW_ID_STATUS_HIGH == low_read1) && (LCD_HW_ID_STATUS_HIGH == high_read1)) {
		/*high status , pull up ID1 ,to prevent electric leakage*/
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_UP);
		if (0 != ret)
			LCD_DEBUG("ID1 mt_set_gpio_pull_select->UP fail\n");

		lcd_id1 = LCD_HW_ID_STATUS_HIGH;
	} else {

		LCD_DEBUG(" Read LCD_id1 error\n");
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_DISABLE);
		if (0 != ret)
			LCD_DEBUG("ID1 mt_set_gpio_pull_select->Disable fail\n");

		lcd_id1 = LCD_HW_ID_STATUS_ERROR;
	}
#ifdef BUILD_LK
	dprintf(CRITICAL, "which_lcd_module_triple,lcd_id0:%d\n", lcd_id0);
	dprintf(CRITICAL, "which_lcd_module_triple,lcd_id1:%d\n", lcd_id1);
#else
	LCD_DEBUG("which_lcd_module_triple,lcd_id0:%d\n", lcd_id0);
	LCD_DEBUG("which_lcd_module_triple,lcd_id1:%d\n", lcd_id1);
#endif
	lcd_id =  lcd_id0 | (lcd_id1 << 2);

#ifdef BUILD_LK
	dprintf(CRITICAL, "which_lcd_module_triple,lcd_id:%d\n", lcd_id);
#else
	LCD_DEBUG("which_lcd_module_triple,lcd_id:%d\n", lcd_id);
#endif

	lcd_id_pins_value = lcd_id;
	return lcd_id;
}
#endif
