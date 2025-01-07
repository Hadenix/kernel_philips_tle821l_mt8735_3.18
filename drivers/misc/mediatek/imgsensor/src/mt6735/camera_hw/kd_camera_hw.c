
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include "kd_camera_hw.h"

#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_camera_feature.h"
#include "elink_devconfig_cam.h"

#include <mach/items.h>

/******************************************************************************
 * Debug configuration
******************************************************************************/
#define PFX "[kd_camera_hw]"
#define PK_DBG_NONE(fmt, arg...)    do {} while (0)
#define PK_DBG_FUNC(fmt, args...)    pr_debug(PFX  fmt, ##args)

#define DEBUG_CAMERA_HW_K
#if 1//def DEBUG_CAMERA_HW_K
#define PK_DBG PK_DBG_FUNC
#define PK_ERR(fmt, arg...) pr_err(fmt, ##arg)
#define PK_XLOG_INFO(fmt, args...)  pr_debug(PFX  fmt, ##args)

#else
#define PK_DBG(a, ...)
#define PK_ERR(a, ...)
#define PK_XLOG_INFO(fmt, args...)
#endif


/* GPIO Pin control*/
struct platform_device *cam_plt_dev = NULL;
struct pinctrl *camctrl = NULL;
struct pinctrl_state *cam0_pnd_h = NULL;
struct pinctrl_state *cam0_pnd_l = NULL;
struct pinctrl_state *cam0_rst_h = NULL;
struct pinctrl_state *cam0_rst_l = NULL;
struct pinctrl_state *cam1_pnd_h = NULL;
struct pinctrl_state *cam1_pnd_l = NULL;
struct pinctrl_state *cam1_rst_h = NULL;
struct pinctrl_state *cam1_rst_l = NULL;
//struct pinctrl_state *cam_ldo0_h = NULL;
//struct pinctrl_state *cam_ldo0_l = NULL;

int mtkcam_gpio_init(struct platform_device *pdev)
{
	int ret = 0;

	camctrl = devm_pinctrl_get(&pdev->dev);
	if (IS_ERR(camctrl)) {
		dev_err(&pdev->dev, "Cannot find camera pinctrl!");
		ret = PTR_ERR(camctrl);
	}
	/*Cam0 Power/Rst Ping initialization */
	cam0_pnd_h = pinctrl_lookup_state(camctrl, "cam0_pnd1");
	if (IS_ERR(cam0_pnd_h)) {
		ret = PTR_ERR(cam0_pnd_h);
		pr_debug("%s : pinctrl err, cam0_pnd_h\n", __func__);
	}

	cam0_pnd_l = pinctrl_lookup_state(camctrl, "cam0_pnd0");
	if (IS_ERR(cam0_pnd_l)) {
		ret = PTR_ERR(cam0_pnd_l);
		pr_debug("%s : pinctrl err, cam0_pnd_l\n", __func__);
	}


	cam0_rst_h = pinctrl_lookup_state(camctrl, "cam0_rst1");
	if (IS_ERR(cam0_rst_h)) {
		ret = PTR_ERR(cam0_rst_h);
		pr_debug("%s : pinctrl err, cam0_rst_h\n", __func__);
	}

	cam0_rst_l = pinctrl_lookup_state(camctrl, "cam0_rst0");
	if (IS_ERR(cam0_rst_l)) {
		ret = PTR_ERR(cam0_rst_l);
		pr_debug("%s : pinctrl err, cam0_rst_l\n", __func__);
	}

	/*Cam1 Power/Rst Ping initialization */
	cam1_pnd_h = pinctrl_lookup_state(camctrl, "cam1_pnd1");
	if (IS_ERR(cam1_pnd_h)) {
		ret = PTR_ERR(cam1_pnd_h);
		pr_debug("%s : pinctrl err, cam1_pnd_h\n", __func__);
	}

	cam1_pnd_l = pinctrl_lookup_state(camctrl, "cam1_pnd0");
	if (IS_ERR(cam1_pnd_l)) {
		ret = PTR_ERR(cam1_pnd_l);
		pr_debug("%s : pinctrl err, cam1_pnd_l\n", __func__);
	}


	cam1_rst_h = pinctrl_lookup_state(camctrl, "cam1_rst1");
	if (IS_ERR(cam1_rst_h)) {
		ret = PTR_ERR(cam1_rst_h);
		pr_debug("%s : pinctrl err, cam1_rst_h\n", __func__);
	}


	cam1_rst_l = pinctrl_lookup_state(camctrl, "cam1_rst0");
	if (IS_ERR(cam1_rst_l)) {
		ret = PTR_ERR(cam1_rst_l);
		pr_debug("%s : pinctrl err, cam1_rst_l\n", __func__);
	}
	/*externel LDO enable */
	/*
	cam_ldo0_h = pinctrl_lookup_state(camctrl, "cam_ldo0_1");
	if (IS_ERR(cam_ldo0_h)) {
		ret = PTR_ERR(cam_ldo0_h);
		pr_debug("%s : pinctrl err, cam_ldo0_h\n", __func__);
	}


	cam_ldo0_l = pinctrl_lookup_state(camctrl, "cam_ldo0_0");
	if (IS_ERR(cam_ldo0_l)) {
		ret = PTR_ERR(cam_ldo0_l);
		pr_debug("%s : pinctrl err, cam_ldo0_l\n", __func__);
	}
	*/
	return ret;
}

int mtkcam_gpio_set(int PinIdx, int PwrType, int Val)
{
	int ret = 0;

	switch (PwrType) {
	case CAMRST:
		if (PinIdx == 0) {
			if (Val == 0)
				pinctrl_select_state(camctrl, cam0_rst_l);
			else
				pinctrl_select_state(camctrl, cam0_rst_h);
		} else {
			if (Val == 0)
				pinctrl_select_state(camctrl, cam1_rst_l);
			else
				pinctrl_select_state(camctrl, cam1_rst_h);
		}
		break;
	case CAMPDN:
		if (PinIdx == 0) {
			if (Val == 0)
				pinctrl_select_state(camctrl, cam0_pnd_l);
			else
				pinctrl_select_state(camctrl, cam0_pnd_h);
		} else {
			if (Val == 0)
				pinctrl_select_state(camctrl, cam1_pnd_l);
			else
				pinctrl_select_state(camctrl, cam1_pnd_h);
		}

		break;
	//case CAMLDO:
		//if (Val == 0)
			//pinctrl_select_state(camctrl, cam_ldo0_l);
		//else
			//pinctrl_select_state(camctrl, cam_ldo0_h);
		//break;
	default:
		PK_DBG("PwrType(%d) is invalid !!\n", PwrType);
		break;
	};

	PK_DBG("PinIdx(%d) PwrType(%d) val(%d)\n", PinIdx, PwrType, Val);

	return ret;
}


/*
#ifndef BOOL
typedef unsigned char BOOL;
#endif
*/

/* Mark: need to verify whether ISP_MCLK1_EN is required in here //Jessy @2014/06/04*/
extern void ISP_MCLK1_EN(BOOL En);
//extern void ISP_MCLK2_EN(BOOL En);
//extern void ISP_MCLK3_EN(BOOL En);

int kdCISModulePowerOn(CAMERA_DUAL_CAMERA_SENSOR_ENUM SensorIdx, char *currSensorName, BOOL On, char* mode_name)
{

	u32 pinSetIdx = 0;//default main sensor

	#define IDX_PS_CMRST 0
	#define IDX_PS_CMPDN 4
	#define IDX_PS_MODE 1
	#define IDX_PS_ON   2
	#define IDX_PS_OFF  3

	#define IDX_PS_AVDD 0
	#define IDX_PS_DVDD 1
	#define IDX_PS_IOVDD 2
	#define IDX_PS_AFVDD 3

	IMG_INFO_STRUCT *cam_list = NULL;
	u32 pmicSet[2][4] = {{0},{0}};
	u32	pinSet[2][8] = {{0},{0}};

	cam_list = Camera_list_config(pinSet, pmicSet, currSensorName);


	if (DUAL_CAMERA_MAIN_SENSOR == SensorIdx) { pinSetIdx = 0; }
	else if (DUAL_CAMERA_SUB_SENSOR == SensorIdx) { pinSetIdx = 1; }

//----------------------------------------------
    //power ON
    if (On) {

		printk("[Elink Camera] %s()  PowerOn ===============\n", __func__);
	
#if 1
         ISP_MCLK1_EN(1);
         //ISP_MCLK2_EN(1);
         //ISP_MCLK3_EN(1);
#else
        if(pinSetIdx == 0 ) {
           ISP_MCLK1_EN(1);
        }
        else if (pinSetIdx == 1) {
            ISP_MCLK1_EN(1);
        }
#endif 

         
		printk("\n kdCISModulePowerOn -on:currSensorName=%s; pinSetIdx=%d\n",currSensorName, pinSetIdx);

		if( Interface_check(currSensorName, pinSetIdx, On, mode_name) != 0 ) { goto _kdCISModulePowerOn_exit_; }

		//Disable active camera
		//if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
		//if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
		//if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
		//if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
		//if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} //high == power down lens module
		//if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //low == reset sensor		
		printk("\n [%s] pinSet[%d][IDX_PS_CMPDN+IDX_PS_OFF] = %d.\n" ,__func__, pinSetIdx, pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF]);
		printk("\n [%s] pinSet[%d][IDX_PS_CMRST+IDX_PS_OFF] = %d.\n" ,__func__, pinSetIdx, pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF]);
		printk("\n [%s] pinSet[%d][IDX_PS_CMPDN+IDX_PS_ON] = %d.\n" ,__func__, pinSetIdx, pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_ON]);
		printk("\n [%s] pinSet[%d][IDX_PS_CMRST+IDX_PS_ON] = %d.\n" ,__func__, pinSetIdx, pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_ON]);

		mtkcam_gpio_set(pinSetIdx, CAMPDN,pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF]);
		mdelay(1);
		mtkcam_gpio_set(pinSetIdx, CAMRST,pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF]);
		mdelay(1);

		//~Disable active camera
		 
		//DOVDD
		printk("\n [kdCISModulePowerOn] [IOVDD = %d] [pinSetIdx = %d] open CAMERA_POWER_VCAM_IO.\n" ,pmicSet[pinSetIdx][IDX_PS_IOVDD] ,pinSetIdx);
		if(TRUE != _hwPowerOnCnt(pmicSet[pinSetIdx][IDX_PS_IOVDD], cam_list->vcam_d2_volt,mode_name)) { PK_DBG("[CAMERA SENSOR] [IOVDD] Fail to enable digital power\n"); }
		mdelay(5);		 
		//AVDD
		printk("\n [kdCISModulePowerOn] [AVDD = %d] [pinSetIdx = %d] open CAMERA_POWER_VCAM_A.\n" ,pmicSet[pinSetIdx][IDX_PS_AVDD] ,pinSetIdx);
		if(TRUE != _hwPowerOnCnt(pmicSet[pinSetIdx][IDX_PS_AVDD], cam_list->vcam_a_volt,mode_name)) { PK_DBG("[CAMERA SENSOR] [AVDD] Fail to enable analog power\n"); }
		mdelay(5);
		//DVDD
		printk("\n [kdCISModulePowerOn] [DVDD = %d] [pinSetIdx = %d] open CAMERA_POWER_VCAM_D.\n" ,pmicSet[pinSetIdx][IDX_PS_DVDD] ,pinSetIdx);
		if(TRUE != _hwPowerOnCnt(pmicSet[pinSetIdx][IDX_PS_DVDD], cam_list->vcam_d_volt,mode_name)) { PK_DBG("[CAMERA SENSOR] [DVDD] Fail to enable digital power\n"); }      
		mdelay(5);	 
		//AF_VCC
		printk("\n [kdCISModulePowerOn] [afvdd = %d] \n" , cam_list->vcam_a2_volt);
		if(cam_list->vcam_a2_volt != 0){
			printk("\n [kdCISModulePowerOn] [AFVDD = %d] [pinSetIdx = %d] open CAMERA_POWER_VCAM_AF.\n" ,pmicSet[pinSetIdx][IDX_PS_AFVDD] ,pinSetIdx);
			if(TRUE != _hwPowerOnCnt(pmicSet[pinSetIdx][IDX_PS_AFVDD], cam_list->vcam_a2_volt,mode_name)) { PK_DBG("[CAMERA SENSOR] [AFVDD] Fail to enable analog power\n"); } 
			mdelay(5);
		}
		//disable inactive sensor
		if (GPIO_CAMERA_INVALID != pinSet[1-pinSetIdx][IDX_PS_CMRST]) {
			//if(mt_set_gpio_mode(pinSet[1-pinSetIdx][IDX_PS_CMRST],pinSet[1-pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
			//if(mt_set_gpio_mode(pinSet[1-pinSetIdx][IDX_PS_CMPDN],pinSet[1-pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
			//if(mt_set_gpio_dir(pinSet[1-pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
			//if(mt_set_gpio_dir(pinSet[1-pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
			//if(mt_set_gpio_out(pinSet[1-pinSetIdx][IDX_PS_CMRST],pinSet[1-pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //low == reset sensor
			//if(mt_set_gpio_out(pinSet[1-pinSetIdx][IDX_PS_CMPDN],pinSet[1-pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} //high == power down lens module
			mtkcam_gpio_set(1-pinSetIdx, CAMRST,pinSet[1-pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF]);
			mtkcam_gpio_set(1-pinSetIdx, CAMPDN,pinSet[1-pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF]);
		}
			
		//enable active sensor
		if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST]) {
			//if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
			//if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
			//if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
			//if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
			//if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
			//if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
			//if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_ON])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
			//if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_ON])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
			mtkcam_gpio_set(pinSetIdx, CAMRST,pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF]);
			mtkcam_gpio_set(pinSetIdx, CAMPDN,pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF]);
			msleep(1);
			mtkcam_gpio_set(pinSetIdx, CAMRST,pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_ON]);
			msleep(5);
			mtkcam_gpio_set(pinSetIdx, CAMPDN,pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_ON]);
			mdelay(5);
		}

	} else {//power OFF

        printk("[Elink Camera] %s()  PowerOff ===============\n", __func__);
#if 1
        ISP_MCLK1_EN(0);
        //ISP_MCLK2_EN(0);
        //ISP_MCLK3_EN(0);
#else
        if(pinSetIdx == 0 ) {
            ISP_MCLK1_EN(0);
        }
        else if (pinSetIdx == 1) {
            ISP_MCLK1_EN(0);
        }
#endif         
		printk("\n kdCISModulePowerOff -off:currSensorName=%s; pinSetIdx=%d\n",currSensorName, pinSetIdx);

		if( Interface_check(currSensorName, pinSetIdx, On, mode_name) != 0 ) { goto _kdCISModulePowerOn_exit_; }

		if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST]) {
	        //if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
	        //if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
	        //if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
	        //if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
	        //if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //low == reset sensor
	    	//if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} //high == power down lens module
			mtkcam_gpio_set(pinSetIdx, CAMRST,pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF]);
			mtkcam_gpio_set(pinSetIdx, CAMPDN,pinSet[pinSetIdx][IDX_PS_CMPDN + IDX_PS_OFF]);
		}

		printk("\n [kdCISModulePowerOn] [AVDD = %d] [pinSetIdx = %d] off CAMERA_POWER_VCAM_A.\n" ,pmicSet[pinSetIdx][IDX_PS_AVDD] ,pinSetIdx);
        if(TRUE != _hwPowerDownCnt(pmicSet[pinSetIdx][IDX_PS_AVDD],mode_name)) { PK_DBG("[CAMERA SENSOR] [AVDD] Fail to OFF analog power\n"); }

		printk("\n [kdCISModulePowerOn] [afvdd = %d] \n" ,cam_list->vcam_a2_volt);
		if(cam_list->vcam_a2_volt != 0){
	        printk("\n [kdCISModulePowerOn] [AFVDD = %d] [pinSetIdx = %d] off CAMERA_POWER_VCAM_AF.\n" ,pmicSet[pinSetIdx][IDX_PS_AFVDD] ,pinSetIdx);
        	if(TRUE != _hwPowerDownCnt(pmicSet[pinSetIdx][IDX_PS_AFVDD],mode_name)) { PK_DBG("[CAMERA SENSOR] [AFVDD] Fail to OFF analog power\n"); }
		}else
			printk("\n [kdCISModulePowerOn] [AFVDD = 0] not setting afvdd,  do without off CAMERA_POWER_VCAM_AF.\n");
		
        printk("\n [kdCISModulePowerOn] [DVDD = %d] [pinSetIdx = %d] off CAMERA_POWER_VCAM_D.\n" ,pmicSet[pinSetIdx][IDX_PS_DVDD] ,pinSetIdx);
        if(TRUE != _hwPowerDownCnt(pmicSet[pinSetIdx][IDX_PS_DVDD], mode_name)) { PK_DBG("[CAMERA SENSOR] [DVDD] Fail to OFF digital power\n"); }
        printk("\n [kdCISModulePowerOn] [IOVDD = %d] [pinSetIdx = %d] off CAMERA_POWER_VCAM_IO.\n" ,pmicSet[pinSetIdx][IDX_PS_IOVDD] ,pinSetIdx);
        if(TRUE != _hwPowerDownCnt(pmicSet[pinSetIdx][IDX_PS_IOVDD],mode_name)) { PK_DBG("[CAMERA SENSOR] [IOVDD] Fail to OFF digital power\n"); }
        
        //For Power Saving
        if(pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF] != GPIO_OUT_ZERO) { 
			//mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN], GPIO_OUT_ZERO); 
			mtkcam_gpio_set(pinSetIdx, CAMPDN,pinSet[pinSetIdx][IDX_PS_CMPDN + IDX_PS_OFF]);
		}
        if(pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF] != GPIO_OUT_ZERO) { 
			//mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST], GPIO_OUT_ZERO); 
			mtkcam_gpio_set(pinSetIdx, CAMRST,pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF]);
		}
        
        // PLS add this
        if(pinSet[1-pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF] != GPIO_OUT_ZERO) { 
			//mt_set_gpio_out(pinSet[1-pinSetIdx][IDX_PS_CMPDN], GPIO_OUT_ZERO); 
			mtkcam_gpio_set(1-pinSetIdx, CAMPDN,pinSet[1-pinSetIdx][IDX_PS_CMPDN + IDX_PS_OFF]);
		}
        if(pinSet[1-pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF] != GPIO_OUT_ZERO) {
			//mt_set_gpio_out(pinSet[1-pinSetIdx][IDX_PS_CMRST], GPIO_OUT_ZERO); 
			mtkcam_gpio_set(1-pinSetIdx, CAMRST,pinSet[1-pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF]);
		}
        //~For Power Saving
    }

    return 0;

_kdCISModulePowerOn_exit_:
    return -EIO;
    
}

EXPORT_SYMBOL(kdCISModulePowerOn);
