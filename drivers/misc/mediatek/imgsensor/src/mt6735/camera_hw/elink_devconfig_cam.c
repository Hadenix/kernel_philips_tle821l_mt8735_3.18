
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
#include "elink_camera_list.h"

#include <linux/regulator/consumer.h>
#include <mach/items.h>

#define VOL_2800 2800000
#define VOL_1800 1800000
#define VOL_1500 1500000
#define VOL_1200 1200000
#define VOL_1000 1000000

int cntVCAMD = 0;
int cntVCAMA = 0;
int cntVCAMIO = 0;
int cntVCAMAF = 0;
int cntVCAMD_SUB = 0;

int mipidvp = 0;
int dualdvp = 0;
int dualmipi = 0;
int diffdvdd = 0;
int afpoweren = 0;
int IS4EC = 0;
char maincam[64];
char subcam[64];
char specialcam[64];
char name_gpio[64] = "gpio";
char name_out0[64] = "_out0";
char name_out1[64] = "_out1";

static unsigned int main_camera_power_avdd = 0;
static unsigned int main_camera_power_dvdd = 0;
static unsigned int main_camera_power_iovdd = 0;
static unsigned int main_camera_power_afvdd = 0;
static unsigned int sub_camera_power_avdd = 0;
static unsigned int sub_camera_power_dvdd = 0;
static unsigned int sub_camera_power_iovdd = 0;
static unsigned int sub_camera_power_afvdd = 0;

struct device *cam_device = NULL;

static DEFINE_SPINLOCK(kdsensor_pw_cnt_lock);


bool _hwPowerOnCnt(KD_REGULATOR_TYPE_T powerId, int powerVolt, char *mode_name)
{

	if (_hwPowerOn(powerId, powerVolt)) {
		spin_lock(&kdsensor_pw_cnt_lock);
		if (powerId == VCAMD)
			cntVCAMD += 1;
		else if (powerId == VCAMA)
			cntVCAMA += 1;
		else if (powerId == VCAMIO)
			cntVCAMIO += 1;
		else if (powerId == VCAMAF)
			cntVCAMAF += 1;
		//else if (powerId == SUB_VCAMD)
		//	cntVCAMD_SUB += 1;
		spin_unlock(&kdsensor_pw_cnt_lock);
		return true;
	}
	return false;
}

bool _hwPowerDownCnt(KD_REGULATOR_TYPE_T powerId, char *mode_name)
{
	if (_hwPowerDown(powerId)) {
		spin_lock(&kdsensor_pw_cnt_lock);
		if (powerId == VCAMD)
			cntVCAMD -= 1;
		else if (powerId == VCAMA)
			cntVCAMA -= 1;
		else if (powerId == VCAMIO)
			cntVCAMIO -= 1;
		else if (powerId == VCAMAF)
			cntVCAMAF -= 1;
		//else if (powerId == SUB_VCAMD)
		//	cntVCAMD_SUB -= 1;
		spin_unlock(&kdsensor_pw_cnt_lock);
		return true;
	}
	return false;
}

void checkPowerBeforClose(char *mode_name)
{
	int i = 0;
	printk
	    ("[checkPowerBeforClose]cntVCAMD:%d, cntVCAMA:%d,cntVCAMIO:%d, cntVCAMAF:%d, cntVCAMD_SUB:%d,\n",
	     cntVCAMD, cntVCAMA, cntVCAMIO, cntVCAMAF, cntVCAMD_SUB);
	for (i = 0; i < cntVCAMD; i++)
		_hwPowerDown(VCAMD);
	for (i = 0; i < cntVCAMA; i++)
		_hwPowerDown(VCAMA);
	for (i = 0; i < cntVCAMIO; i++)
		_hwPowerDown(VCAMIO);
	for (i = 0; i < cntVCAMAF; i++)
		_hwPowerDown(VCAMAF);
	//for (i = 0; i < cntVCAMD_SUB; i++)
	//	_hwPowerDown(SUB_VCAMD);
	cntVCAMD = 0;
	cntVCAMA = 0;
	cntVCAMIO = 0;
	cntVCAMAF = 0;
	cntVCAMD_SUB = 0;
}

int Handle_iovdd(bool Open, int pinSetIdx, char* mode_name)
{
	if(Open) {
		//if(TRUE != hwPowerOn(pinSetIdx?sub_camera_power_iovdd:main_camera_power_iovdd, VOL_1800,mode_name)) { PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n"); return -1; }
		if (TRUE != _hwPowerOnCnt(VCAMIO, VOL_1800, mode_name)){ printk("[CAMERA SENSOR] Fail to enable digital power\n"); return -1; }
		mdelay(5);
	} else {
		//if(TRUE != hwPowerDown(pinSetIdx?sub_camera_power_iovdd:main_camera_power_iovdd,mode_name)) { PK_DBG("[CAMERA SENSOR] [IOVDD] Fail to OFF digital power\n"); return -1; }
		if (TRUE != _hwPowerDownCnt(VCAMIO, mode_name)) { printk("[CAMERA SENSOR] [IOVDD] Fail to OFF digital power\n"); return -1; }
		mdelay(5);
	}
	return 0;
}

int IS_MIPI_CAM_Project(char *currSensorName)
{
  char *index1=NULL;
  
  index1 = strstr(currSensorName,"mipi");  
  printk("[IS_MIPI_CAM_Project] index1 = %s currSensorName = %s \n", index1, currSensorName);
  if (index1==NULL) {
  	return 0;
  }else {
  	return 1;
  }

}

int Interface_check(char *currSensorName, int pinSetIdx, int On, char* mode_name)
{
	if(mipidvp) {
		if (pinSetIdx == 0 && IS_MIPI_CAM_Project(currSensorName) == 0) { 
			printk("Main sensor not use [%s] dvp driver. \n", currSensorName); 
			Handle_iovdd(On, pinSetIdx, mode_name);
			return -1; 
		} else if(pinSetIdx == 1 && IS_MIPI_CAM_Project(currSensorName) == 1) {
			printk("Sub sensor not use [%s] mipi driver. \n", currSensorName);
			Handle_iovdd(On, pinSetIdx, mode_name);
			return -1; 
		}
	} else if (dualdvp) {
		if (IS_MIPI_CAM_Project(currSensorName) == 1) { 
			printk("Sensor not use [%s] mipi driver. \n", currSensorName); 
			Handle_iovdd(On, pinSetIdx, mode_name);
			return -1; 
		}
	} else if (dualmipi) {
		if (IS_MIPI_CAM_Project(currSensorName) == 0) { 
			printk("Sensor not use [%s] dvp driver. \n", currSensorName); 
			Handle_iovdd(On, pinSetIdx, mode_name);
			return -1; 
		}
	} 

	return 0;
}


#if 0
inline void STRSPLICE(char *dest, char *a, char *b, char *c)
{
	strcpy(dest, a);
	strcat(dest, b);
	strcat(dest, c);
}
#else
#define STRSPLICE(dest, a, b, c) \
	do{ \
		strcpy(dest, a); \
		strcat(dest, b); \
		strcat(dest, c); \
	} while(0)
#endif

/*
 * items.ini file
 * for example:
 * camera.main.rst.pin				11
 * camera.main.pwd.pin				12
 * camera.sub.rst.pin				11
 * camera.sub.pwd.pin				7
 */
int _cam_items_gpio_config(char *items_key)
{
	char STR_GPIO[64];
	char DTS_GPIO_OUT0[64];
	char DTS_GPIO_OUT1[64];
	int ret = 0;

	if(item_exist(items_key)) {
		item_string(STR_GPIO, items_key, 0);
		STRSPLICE(DTS_GPIO_OUT0, name_gpio, STR_GPIO, name_out0);
		STRSPLICE(DTS_GPIO_OUT1, name_gpio, STR_GPIO, name_out1);
		printk("[%s] [GPIO%s] DTS_GPIO_OUT0 = %s, DTS_GPIO_OUT1 = %s\n", __func__, STR_GPIO, DTS_GPIO_OUT0, DTS_GPIO_OUT1);
		printk("[%s] items_key = %s\n", __func__, items_key);

		if(!strcmp(CMRST0, items_key)) {
			cam0_rst_h = pinctrl_lookup_state(camctrl, DTS_GPIO_OUT1);
			if (IS_ERR(cam0_rst_h)) {
				ret = PTR_ERR(cam0_rst_h);
				printk("%s : pinctrl err, e_cam0_rst_h\n", __func__);
			}
			cam0_rst_l = pinctrl_lookup_state(camctrl, DTS_GPIO_OUT0);
			if (IS_ERR(cam0_rst_l)) {
				ret = PTR_ERR(cam0_rst_l);
				printk("%s : pinctrl err, e_cam0_rst_l\n", __func__);
			}
		} else if(!strcmp(CMPDN0, items_key)) {
			cam0_pnd_h = pinctrl_lookup_state(camctrl, DTS_GPIO_OUT1);
			if (IS_ERR(cam0_pnd_h)) {
				ret = PTR_ERR(cam0_pnd_h);
				printk("%s : pinctrl err, e_cam0_pnd_h\n", __func__);
			}
			cam0_pnd_l = pinctrl_lookup_state(camctrl, DTS_GPIO_OUT0);
			if (IS_ERR(cam0_pnd_l)) {
				ret = PTR_ERR(cam0_pnd_l);
				printk("%s : pinctrl err, e_cam0_pnd_l\n", __func__);
			}
		} else if(!strcmp(CMRST1, items_key)) {
			cam1_rst_h = pinctrl_lookup_state(camctrl, DTS_GPIO_OUT1);
			if (IS_ERR(cam1_rst_h)) {
				ret = PTR_ERR(cam1_rst_h);
				printk("%s : pinctrl err, e_cam1_rst_h\n", __func__);
			}
			cam1_rst_l = pinctrl_lookup_state(camctrl, DTS_GPIO_OUT0);
			if (IS_ERR(cam1_rst_l)) {
				ret = PTR_ERR(cam1_rst_l);
				printk("%s : pinctrl err, e_cam1_rst_l\n", __func__);
			}
		} else if(!strcmp(CMPDN1, items_key)) {
			cam1_pnd_h = pinctrl_lookup_state(camctrl, DTS_GPIO_OUT1);
			if (IS_ERR(cam1_pnd_h)) {
				ret = PTR_ERR(cam1_pnd_h);
				printk("%s : pinctrl err, e_cam1_pnd_h\n", __func__);
			}
			cam1_pnd_l = pinctrl_lookup_state(camctrl, DTS_GPIO_OUT0);
			if (IS_ERR(cam1_pnd_l)) {
				ret = PTR_ERR(cam1_pnd_l);
				printk("%s : pinctrl err, e_cam1_pnd_l\n", __func__);
			}
		}
	} else {
		printk("[%s] \"%s\" not exist in items.ini !!! \n", __func__, items_key);
	} 

	return ret;
}

/*
 * items.ini file
 * for example:
 * camera.main.power.avdd 	vcama
 * camera.sub.power.avdd 	vcamd
 * camera.main.power.dvdd 	vcamio
 * camera.sub.power.dvdd 	vcamaf
 * camera.main.power.iovdd 	vcama
 * camera.sub.power.iovdd 	vcamd
 * camera.main.power.afvdd 	vcamio
 * camera.sub.power.afvdd 	vcamaf
 */
bool _cam_pmic_config(char *items_key)
{
	char STR_PMIC[64];
	struct device_node *node = NULL;
		
	/* check if customer camera node defined */
	printk("[%s]\n", __func__);
	node = of_find_compatible_node(NULL, NULL, "mediatek,camera_hw");
	if (node) {
		item_string(STR_PMIC, items_key, 0);
		printk("[%s] items_key = %s, STR_PMIC = %s .\n", __func__, items_key, STR_PMIC);
		if(!strcmp(MAIN_AVDD, items_key)) {
			regVCAMA = regulator_get(sensor_device, STR_PMIC);
			main_camera_power_avdd = VCAMA;
		} else if(!strcmp(MAIN_DVDD, items_key)) {
			regVCAMD = regulator_get(sensor_device, STR_PMIC);
			main_camera_power_dvdd = VCAMD;
		} else if(!strcmp(MAIN_IOVDD, items_key)) {
			regVCAMIO = regulator_get(sensor_device, STR_PMIC);
			main_camera_power_iovdd = VCAMIO;
		} else if(!strcmp(MAIN_AFVDD, items_key)) {
			regVCAMAF = regulator_get(sensor_device, STR_PMIC);
			main_camera_power_afvdd = VCAMAF;
		} else if(!strcmp(SUB_AVDD, items_key)) {
			//regSubVCAMA = regulator_get(sensor_device, STR_PMIC);
			sub_camera_power_avdd = VCAMA;
		} else if(!strcmp(SUB_DVDD, items_key)) {
			//regSubVCAMD = regulator_get(sensor_device, STR_PMIC);
			sub_camera_power_dvdd = VCAMD;
		} else if(!strcmp(SUB_IOVDD, items_key)) {
			//regSubVCAMIO = regulator_get(sensor_device, STR_PMIC);
			sub_camera_power_iovdd = VCAMIO;
		} else if(!strcmp(SUB_AFVDD, items_key)) {
			//regSubVCAMAF = regulator_get(sensor_device, STR_PMIC);
			sub_camera_power_afvdd = VCAMAF;
		} else {
			printk("[%s] regulator get cust camera node failed!\n", __func__);
			return FALSE;
		}
	} else {
		printk("[%s] can not find node!\n", __func__);	
		return FALSE;
	}
	return TRUE;

}

int Cam_dts_gpio_config(void)
{

	camctrl = devm_pinctrl_get(cam_device);
	if (IS_ERR(camctrl)) {
		printk("[%s] Cannot find camera pinctrl!", __func__);
	}

	/*config sub rst pin*/
	/*if(item_exist("camera.sub.rst.pin"))
		{pin = item_integer("camera.sub.rst.pin",0);gpio_camera_cmrst1_pin = (pin | 0x80000000);}
	else*/
		//gpio_camera_cmrst1_pin = GPIO_CAMERA_CMRST1_PIN;
	_cam_items_gpio_config("camera.sub.rst.pin");

	/*config sub pwd pin*/
	/*if(item_exist("camera.sub.pwd.pin"))
		{pin = item_integer("camera.sub.pwd.pin",0);gpio_camera_cmpdn1_pin = (pin | 0x80000000);}
	else*/
		//gpio_camera_cmpdn1_pin = GPIO_CAMERA_CMPDN1_PIN;
	_cam_items_gpio_config("camera.sub.pwd.pin");

	/*config main rst pin*/
	/*if(item_exist("camera.main.rst.pin"))
		{pin = item_integer("camera.main.rst.pin",0);gpio_camera_cmrst_pin = (pin | 0x80000000);}
	else*/
		//gpio_camera_cmrst_pin = GPIO_CAMERA_CMRST_PIN;
	_cam_items_gpio_config("camera.main.rst.pin");

	/*config main pwd pin*/
	/*if(item_exist("camera.main.pwd.pin"))
		{pin = item_integer("camera.main.pwd.pin",0);gpio_camera_cmpdn_pin = (pin | 0x80000000);}
	else*/
		//gpio_camera_cmpdn_pin = GPIO_CAMERA_CMPDN_PIN;
	_cam_items_gpio_config("camera.main.pwd.pin");

	/*config af power pin*/
	/*if(item_exist("camera.af.power.pin"))
		{pin = item_integer("camera.af.power.pin",0);gpio_af_power_en_pin = (pin | 0x80000000); afpoweren = 1;}
	else
		{gpio_af_power_en_pin = GPIO21 | 0x80000000 ; afpoweren = 0;}*/

	return 0;

}

/*
 * items.ini file
 * for example:
 * camera.main.interface			dvp
 * camera.sub.interface				dvp
 */
int Cam_interface_config(void)
{
	if(item_exist("camera.main.interface") && item_exist("camera.sub.interface")) {	
		item_string(maincam,"camera.main.interface",0);
		item_string(subcam,"camera.sub.interface",0);
		if( (0 == strcmp(maincam, "mipi")) && (0 == strcmp(subcam, "mipi")) ) {
			mipidvp = 0; dualdvp = 0; dualmipi = 1; 
		} else if( (0 == strcmp(maincam, "dvp")) && (0 == strcmp(subcam, "dvp")) ) {
			mipidvp = 0; dualdvp = 1; dualmipi = 0; 
		} else if( (0 == strcmp(maincam, "mipi")) && (0 == strcmp(subcam, "dvp")) ) { 
			mipidvp = 1; dualdvp = 0; dualmipi = 0; 
		}
		printk("\n[%s] maincam is %s, subcam is %s. \n", __func__, maincam, subcam);
	} else { 
		mipidvp = 0; dualdvp = 0; dualmipi = 0; 
	}

	return 0;
}

int Cam_pmic_config(void)
{
	int temp = 0;

	temp = item_exist("camera.main.power.avdd") & item_exist("camera.sub.power.avdd") \
			& item_exist("camera.main.power.dvdd") & item_exist("camera.sub.power.dvdd") \
			& item_exist("camera.main.power.iovdd") & item_exist("camera.sub.power.iovdd") \
			& item_exist("camera.main.power.afvdd") & item_exist("camera.sub.power.afvdd");
	if(temp) {
		printk("\n[%s] avdd, dvdd, iovdd, afvdd all exist in items. \n", __func__);
		_cam_pmic_config("camera.main.power.avdd");
		_cam_pmic_config("camera.sub.power.avdd");
		_cam_pmic_config("camera.main.power.dvdd");
		_cam_pmic_config("camera.sub.power.dvdd");
		_cam_pmic_config("camera.main.power.iovdd");
		_cam_pmic_config("camera.sub.power.iovdd");
		_cam_pmic_config("camera.main.power.afvdd");
		_cam_pmic_config("camera.sub.power.afvdd");
	} else {
		printk("\n[%s] avdd, dvdd, iovdd, afvdd use default value. \n", __func__);
		main_camera_power_avdd = VCAMA;
		main_camera_power_dvdd = VCAMD;
		main_camera_power_iovdd = VCAMIO;
		main_camera_power_afvdd = VCAMAF;
		sub_camera_power_avdd = VCAMA;
		sub_camera_power_dvdd = VCAMD;
		sub_camera_power_iovdd = VCAMIO;
		sub_camera_power_afvdd = VCAMAF;
	}

	return 0;
}

int Elink_items_config_CAM(void)
{

	Cam_dts_gpio_config();

	Cam_interface_config();

	Cam_pmic_config();

	return 0;
}

IMG_INFO_STRUCT* Camera_list_config(u32 (*pinSet)[8], u32 (*pmicSet)[4], char *currSensorName)
{
	int i;
	int sizeoflist;

	sizeoflist = sizeof(camera_list) / sizeof(IMG_INFO_STRUCT);
	for (i = 0; i < sizeoflist; i++)
	{
		if(strcmp(currSensorName, camera_list[i].name) == 0)
			break;
	}

	if (i == sizeoflist)
	{
		printk("list of designated camera isn't exist currSensorName =%s\n", currSensorName);
		return NULL;
	}

	pinSet[IDX_MAIN][IDX_PS_CMRST+IDX_PS_ON] = camera_list[i].rst_on;
	pinSet[IDX_MAIN][IDX_PS_CMRST+IDX_PS_OFF] = camera_list[i].rst_off;
	pinSet[IDX_MAIN][IDX_PS_CMPDN+IDX_PS_ON] = camera_list[i].pdn_on;
	pinSet[IDX_MAIN][IDX_PS_CMPDN+IDX_PS_OFF] = camera_list[i].pdn_off;

	pinSet[IDX_SUB][IDX_PS_CMRST+IDX_PS_ON] = camera_list[i].rst_on;
	pinSet[IDX_SUB][IDX_PS_CMRST+IDX_PS_OFF] = camera_list[i].rst_off;
	pinSet[IDX_SUB][IDX_PS_CMPDN+IDX_PS_ON] = camera_list[i].pdn_on;
	pinSet[IDX_SUB][IDX_PS_CMPDN+IDX_PS_OFF] = camera_list[i].pdn_off;

	pmicSet[IDX_MAIN][IDX_PS_AVDD] = main_camera_power_avdd;
	pmicSet[IDX_MAIN][IDX_PS_DVDD] = main_camera_power_dvdd;
	pmicSet[IDX_MAIN][IDX_PS_IOVDD] = main_camera_power_iovdd;
	pmicSet[IDX_MAIN][IDX_PS_AFVDD] = main_camera_power_afvdd;

	pmicSet[IDX_SUB][IDX_PS_AVDD] = sub_camera_power_avdd;
	pmicSet[IDX_SUB][IDX_PS_DVDD] = sub_camera_power_dvdd;
	pmicSet[IDX_SUB][IDX_PS_IOVDD] = sub_camera_power_iovdd;
	pmicSet[IDX_SUB][IDX_PS_AFVDD] = sub_camera_power_afvdd;

	return &camera_list[i];
}

int Elink_cam_device_get(struct platform_device *pdev)
{
	cam_device = &pdev->dev;
	return 0;
}
