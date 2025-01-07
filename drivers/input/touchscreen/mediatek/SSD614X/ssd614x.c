/*
 * Copyright 2016 Solomon Systech Ltd. All rights reserved.
 *
 * Author: binkazhang@solomon-systech.com
 * Date: 2016.01.21
 */
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/mutex.h>
#include <linux/interrupt.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/input/mt.h>
#include <asm/uaccess.h>
#include <linux/firmware.h>

#include "ssd614x.h"
#include "tpd.h"

#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>

static u8 m_up_event[SSL_MAX_POINT] = {0,};
static const struct i2c_device_id tpd_ssl_id[] ={{TPD_DEVICE,0}, {}};
static struct of_device_id ssd6141_match_table[] = {
	{ .compatible = "mediatek,cap_touch"},
	{ },
};

#define TPD_KEY_COUNT	3
#define TPD_KEYS			{KEY_BACK,KEY_HOMEPAGE,KEY_MENU}

//#define  TPD_HAVE_BUTTON

#ifdef TPD_HAVE_BUTTON
static int tpd_keys_local[TPD_KEY_COUNT] = TPD_KEYS;

static int ssl_send_key(struct input_dev *input, unsigned int key, int level);
#endif

/* to support I2C read/write */
struct I2C_func {
	int (*ts_read_data)(struct i2c_client *, u16, u8 *, u16);
//	int (*ts_read_data_ex)(struct i2c_client *, u8 *, u16, u8 *, u16);
	int (*ts_write_data)(struct i2c_client *, u16, u8 *, u16);
};

struct ssl_point {
	unsigned short id_x;
	unsigned short w_y;
};

struct ssl_data {
	unsigned short point_info;
	struct ssl_point points[SSL_MAX_POINT];
	unsigned short rawData[SSL_MAX_RAWDATA_QUEUE][SSL_MAX_NODE + 4*SSL_MAX_POINT + 2];
	int queue_front;
	int queue_rear;
	int validPointCnt;
	int lastValidCnt;
	unsigned int keydata;
};

struct ssl_config {
	unsigned short fw_ver;
	unsigned short data_ver;
	unsigned short max_x;		// x resolution
	unsigned short max_y;		// y resolution
	unsigned short using_point;
	unsigned short x_node;
	unsigned short y_node;
	bool check;
};

enum _ts_work_procedure {
	TS_NO_WORK = 0,
	TS_NORMAL_WORK,
	TS_ESD_TIMER_WORK,
	TS_IN_EALRY_SUSPEND,
	TS_IN_SUSPEND,
	TS_IN_RESUME,
	TS_IN_LATE_RESUME,
	TS_IN_UPGRADE,
	TS_REMOVE_WORK,
	TS_SET_MODE,
	TS_GET_RAW_DATA,
};

struct ssl_device {
	struct input_dev *input_dev;
	struct i2c_client *client;
	struct atc260x_dev *atc260x;
	struct ssl_data *ftdata;
	struct ssl_config *ftconfig;
	struct work_struct work;
	struct workqueue_struct *workqueue;
	struct mutex lock;
	int irq;
	int int_pin;
	int reset_pin;
	
	unsigned short touch_mode;
	unsigned char update;
	struct semaphore	raw_data_lock;

	unsigned char work_procedure;
	struct semaphore work_procedure_lock;
	struct I2C_func i2c_func;
};

struct _raw_ioctl {
	int sz;
	unsigned char *buf;
};

struct _reg_ioctl {
	int addr;
	int *val;
};

struct _down_ioctl {
	int sz;
	char *file;
};
#ifdef SUPPORT_KEY_BUTTON
static int lpm_gesture_keys[] = { KEY_POWER, KEY_BACK, KEY_WAKEUP, KEY_MENU, KEY_HOMEPAGE };
#define LPM_GESTURE_KEY_CNT	(sizeof(lpm_gesture_keys)/sizeof(lpm_gesture_keys[0]))
#endif
#ifdef SUPPORT_GESTURE
#define POWER_STATUS_NM				0x8000
#define POWER_STATUS_LPM			0x4000
#define LPM_RESUME 					(POWER_STATUS_NM | 0x0001)
#define LPM_SUSPEND 				(POWER_STATUS_LPM | 0x0001)
#define LPM_SUSPEND_DELAY 			(POWER_STATUS_LPM | 0x0002)

static volatile int g_power_status = LPM_RESUME;		// 0:resume , 1:suspend
#endif

extern struct tpd_device* tpd;

int g_debug_show = 1;
struct ssl_device *g_ftdev = NULL;

struct ssl_device *g_misc_dev = NULL;
int g_queue_count = 0;
struct i2c_client *g_misc_client = NULL;
unsigned int tpd_rst_gpio_number = 0;
unsigned int tpd_int_gpio_number = 0;
unsigned int touch_irq = 0;
unsigned tpd_intr_type = 0;

static int ssl_hw_init(void);
static void ssl_hw_deint(void);
int ssl_init(struct ssl_device *g_ftdev);

/************************************************************
* rawdata queue process
*************************************************************/
static char *get_rear_queue_buff(struct ssl_data *ftdata) {
	if ((ftdata->queue_rear+1)%SSL_MAX_RAWDATA_QUEUE == ftdata->queue_front) return NULL;

	return (char *)&(ftdata->rawData[(ftdata->queue_rear+1)%SSL_MAX_RAWDATA_QUEUE][0]);
}

static int put_queue(struct ssl_data *ftdata) {
	if ((ftdata->queue_rear+1)%SSL_MAX_RAWDATA_QUEUE == ftdata->queue_front) return -1;
	ftdata->queue_rear = (ftdata->queue_rear+1)%SSL_MAX_RAWDATA_QUEUE;

	return 0;
}

static unsigned char * get_front_queue_buff(struct ssl_data *ftdata) {
	int tmp = ftdata->queue_front;

	if (ftdata->queue_front == ftdata->queue_rear) return NULL;

	tmp = (ftdata->queue_front + 1)%SSL_MAX_RAWDATA_QUEUE;

	return (unsigned char *)&(ftdata->rawData[tmp][0]);
}

static int get_queue(struct ssl_data *ftdata) {
	if (ftdata->queue_front == ftdata->queue_rear) return -1;

	ftdata->queue_front = (ftdata->queue_front + 1)%SSL_MAX_RAWDATA_QUEUE;

	return 0;
}

//************************************************************
//I2C interface
//************************************************************
int ts_read_data(struct i2c_client *client, unsigned short reg, unsigned char *values, unsigned short length)
{
	s32 ret;
#ifdef SUPPORT_I2C_READ_8BYTE
	int left = length;
	u8 *pData = values;
	while (left > 0) {
			/* select register*/
			ret = i2c_master_send(client, (u8 *)&reg, 2);
			if (ret < 0) {
				printk("I2C READ CMD FAIL");
				return ret;
			}
		
			/* for setup tx transaction. */
			udelay(DELAY_FOR_TRANSCATION);
			if(left > MAX_TRANSACTION_LENGTH)
					ret = i2c_master_recv(client, pData, MAX_TRANSACTION_LENGTH);
			else
					ret = i2c_master_recv(client, pData, left);
		
			SSL_DEBUG("ts_read_data left = %d (0x%04X)",left,reg);
			if (ret < 0) {
					printk("I2C READ DATA FAIL (%d) (0x%04X)", ret, reg);
					return ret;
			}
			left -= MAX_TRANSACTION_LENGTH;
			pData += MAX_TRANSACTION_LENGTH;
			udelay(DELAY_FOR_POST_TRANSCATION);
	}
#else
		/* select register*/
		ret = i2c_master_send(client, (u8 *)&reg, 2);
		if (ret < 0) {
			printk("I2C READ CMD FAIL");
			return ret;
		}
	
		/* for setup tx transaction. */
		udelay(DELAY_FOR_TRANSCATION);

		ret = i2c_master_recv(client, values, length);

		if (ret < 0) {
			printk("I2C READ DATA FAIL (%d) (0x%04X)", ret, reg);
			return ret;
		}
	udelay(DELAY_FOR_POST_TRANSCATION);
#endif
	return length;
}

int ts_write_data(struct i2c_client *client, unsigned short reg, unsigned char *values, unsigned short length)
{
	s32 ret;
	s32 retry = 2;
	u8 pkt[1026];				/* max packet */

	memset(pkt, 0, 1026);

	pkt[0] = (reg)&0xff;			/* reg addr */
	pkt[1] = (reg >> 8)&0xff;
	memcpy((u8 *)&pkt[2], values, length);

again:
	ret = i2c_master_send(client, pkt, length + 2);
	if (ret < 0) {
		printk("I2C WRITE FAIL : 0x%x", reg);
		if ((retry--) > 0)
			goto again;

		return ret;
	}

	udelay(DELAY_FOR_POST_TRANSCATION);

	return length;
}

//************************************************************
//debug interface
//************************************************************
static ssize_t str_read_version(struct device *dev, struct device_attribute *attr, char *buf)
{
	char data[32];
	ssize_t ret;

	unsigned short fw_ver;
	unsigned short data_ver;

	ret = ts_read_data(g_misc_dev->client, SSL_FW_VERSION, (unsigned char *)&(fw_ver), 2);
	if (ret < 0) {
		printk("error: read SSL_FW_VERSION error.\n");
	}

	ret = ts_read_data(g_misc_dev->client, SSL_USR_VERSION, (unsigned char *)&(data_ver), 2);
	if (ret < 0) {
		printk("error: read SSL_USR_VERSION error.\n");
	}

	sprintf(data,"FW version : 0x%04X  USR version : 0x%04X ",fw_ver, data_ver);
	ret = snprintf(buf,PAGE_SIZE,"%s\n",data);
	return ret;
}
static DEVICE_ATTR(version, S_IRUGO,str_read_version, NULL);

static char ssd_return_str[256000]={0};

#define SSDTOUCH_WRITE				"write"
#define SSDTOUCH_READ				"read"

#define SSDTOUCH_DEBUGSHOW			"debugshow"
#define SSDTOUCH_RESET				"reset"
#define SSDTOUCH_UPDATE				"update"
#define SSDTOUCH_FORCE				"force"
#define SSDTOUCH_VERSION			"version"

static ssize_t ssdtouch_initcode_set(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	char *value=NULL;
	unsigned int reg=0;
	unsigned int data=0;

	unsigned short send_reg=0;
	unsigned short send_data=0;
	char tmp_str[64]={0};

	const char *ssd_receive_buf=buf;

	if ((value=strstr(ssd_receive_buf, SSDTOUCH_WRITE)) != NULL)//write register
	{
		value += strlen(SSDTOUCH_WRITE);
		if (strlen(value)<14)
		{
			sprintf(ssd_return_str, "%s error!\n",SSDTOUCH_WRITE);
			return -EFAULT;
		}
		value += strlen(" ");
		sscanf(value, "0x%x 0x%x", &reg, &data);
		send_reg = reg;
		send_data = data;

		ts_write_data(g_misc_client, send_reg, (unsigned char *)&(send_data), 2);

		memset(ssd_return_str, 0 ,sizeof(ssd_return_str));
		sprintf(ssd_return_str, "write [0x%04X]=0x%04X\n", reg, data);
	}
	else if ((value=strstr(ssd_receive_buf, SSDTOUCH_READ)) != NULL) //read register
	{
		value += strlen(SSDTOUCH_READ);
		if (strlen(value)<4)
		{
			sprintf(ssd_return_str, "%s error!\n",SSDTOUCH_READ);
			return -EFAULT;
		}

		value += strlen(" ");
		sscanf(value, "0x%x", &reg);
		send_reg = reg;

		ts_read_data(g_misc_client, send_reg, (unsigned char *)&(send_data), 2);

		memset(ssd_return_str, 0 ,sizeof(ssd_return_str));
		sprintf(ssd_return_str, "0x%04X\n", send_data);
	}
	else if ((value=strstr(ssd_receive_buf, SSDTOUCH_DEBUGSHOW)) != NULL) //debugshow
	{
		value += strlen(SSDTOUCH_DEBUGSHOW);
		if (strlen(value)<4)
		{
			sprintf(ssd_return_str, "%s error!\n",SSDTOUCH_DEBUGSHOW);
			return -EFAULT;
		}
		value += strlen(" ");
		sscanf(value, "0x%x", &reg);
		g_debug_show = reg;

		memset(ssd_return_str, 0 ,sizeof(ssd_return_str));
		sprintf(ssd_return_str, "%s g_debug_show=%d ok!\n",SSDTOUCH_DEBUGSHOW, g_debug_show);
	}
	else if ((value=strstr(ssd_receive_buf, SSDTOUCH_UPDATE)) != NULL) //update
	{
		dl_force_upgrade_by_data();
		sprintf(ssd_return_str, "%s ok!\n",SSDTOUCH_UPDATE);
	}
	else if ((value=strstr(ssd_receive_buf, SSDTOUCH_FORCE)) != NULL) //force
	{
		dl_force_upgrade_by_default_file();
		sprintf(ssd_return_str, "%s ok!\n",SSDTOUCH_FORCE);
	}
	else if ((value=strstr(ssd_receive_buf, SSDTOUCH_RESET)) != NULL) //reset
	{
		ssl_init(g_ftdev);
		memset(ssd_return_str, 0 ,sizeof(ssd_return_str));
		memset(tmp_str, 0 ,sizeof(tmp_str));sprintf(tmp_str, "################################\n");strcat(ssd_return_str, tmp_str);
		memset(tmp_str, 0 ,sizeof(tmp_str));sprintf(tmp_str, " fw version : %d\n", g_ftdev->ftconfig->fw_ver);strcat(ssd_return_str, tmp_str);
		memset(tmp_str, 0 ,sizeof(tmp_str));sprintf(tmp_str, "usr version : %d\n", g_ftdev->ftconfig->data_ver);strcat(ssd_return_str, tmp_str);
		memset(tmp_str, 0 ,sizeof(tmp_str));sprintf(tmp_str, " resolution : %d X %d\n", g_ftdev->ftconfig->max_x, g_ftdev->ftconfig->max_y);strcat(ssd_return_str, tmp_str);
		memset(tmp_str, 0 ,sizeof(tmp_str));sprintf(tmp_str, "  node info : %d X %d\n", g_ftdev->ftconfig->x_node, g_ftdev->ftconfig->y_node);strcat(ssd_return_str, tmp_str);
		memset(tmp_str, 0 ,sizeof(tmp_str));sprintf(tmp_str, "using point : %d\n", g_ftdev->ftconfig->using_point);strcat(ssd_return_str, tmp_str);
		memset(tmp_str, 0 ,sizeof(tmp_str));sprintf(tmp_str, "#################################\n");strcat(ssd_return_str, tmp_str);
	}
	return 1;
}

static ssize_t ssdtouch_initcode_get(struct device *dev, struct device_attribute *attr,  char *buf)
{
   return snprintf(buf, PAGE_SIZE, "%s\n", ssd_return_str);
}

static DEVICE_ATTR(ssdtouch, 0660, ssdtouch_initcode_get, ssdtouch_initcode_set);


static inline s32 ssl_clear_int(struct i2c_client *client)
{
	int val = 0x01;

	return ts_write_data(client, SSL_INT_CLEAR_CMD, (u8 *)&val, 2);
}

/* Check interrupt pin */
static s32 int_pin_check(struct ssl_device *ftdev, int retry)
{
	int ret = 0;

	if (g_ftdev == NULL)
		return 0;

	if (retry == 0)
		return 0;

	do {
		gpio_direction_input(tpd_int_gpio_number);
		if (gpio_get_value(g_ftdev->int_pin) == 0)
			break;

		mdelay(1);
	} while ((retry--) > 0);

	if (retry < 1)
		ret = -1;

	return ret;
}
static int ts_write_touchmode(unsigned short value)
{
	int err;

	down(&g_misc_dev->work_procedure_lock);
	if (g_misc_dev->work_procedure != TS_NO_WORK) {
		printk("error: other process occupied.. (%d)\n", g_misc_dev->work_procedure);
		up(&g_misc_dev->work_procedure_lock);
		return -1;
	}

	g_misc_dev->work_procedure = TS_SET_MODE;

	err = ts_write_data(g_misc_dev->client,SSL_TOUCH_MODE, (unsigned char *)&(value), 2);
	if (err < 0)
		printk("error: failed to set TOUCH_MODE %d.\n" , g_misc_dev->touch_mode);

	err = ts_read_data(g_misc_dev->client, SSL_TOUCH_MODE, (unsigned char *)&(g_misc_dev->touch_mode), 2);
	if (err < 0)
		printk("error: read touch_mode\n");

	SSL_DEBUG("touch mode %d %d", value, g_misc_dev->touch_mode);

	g_misc_dev->work_procedure = TS_NO_WORK;

	ssl_clear_int(g_ftdev->client);

	up(&g_misc_dev->work_procedure_lock);

	return err;
}

//************************************************************
// misc IOCTL Driver
//************************************************************
static int ts_misc_fops_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int ts_misc_fops_release(struct inode *inode, struct file *file)
{
	return 0;
}

static long ts_misc_fops_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	void __user *argp = (void __user *)arg;

	struct _raw_ioctl raw_ioctl;
	struct _reg_ioctl reg_ioctl;
	struct _down_ioctl down_ioctl;

	unsigned char * charData;
	int ret = 0;
	unsigned short val;
	int nval = 0;

	char data[256];

	switch (cmd)
	{
		case TOUCH_IOCTL_GET_FW_VERSION:
			ret = g_misc_dev->ftconfig->fw_ver;
			if (copy_to_user(argp, &ret, sizeof(ret)))
				return -1;
			return ret;

		case TOUCH_IOCTL_GET_DATA_VERSION:
			ret = g_misc_dev->ftconfig->data_ver;
			if (copy_to_user(argp, &ret, sizeof(ret)))
				return -1;
			return ret;

		case TOUCH_IOCTL_GET_X_RESOLUTION:
			ret = g_misc_dev->ftconfig->max_x;
			if (copy_to_user(argp, &ret, sizeof(ret)))
				return -1;
			return ret;

		case TOUCH_IOCTL_GET_Y_RESOLUTION:
			ret = g_misc_dev->ftconfig->max_y;
			if (copy_to_user(argp, &ret, sizeof(ret)))
				return -1;
			return ret;

		case TOUCH_IOCTL_GET_X_NODE_NUM:
			ret = g_misc_dev->ftconfig->x_node;
			if (copy_to_user(argp, &ret, sizeof(ret)))
				return -1;
			return ret;

		case TOUCH_IOCTL_GET_Y_NODE_NUM:
			ret = g_misc_dev->ftconfig->y_node;
			if (copy_to_user(argp, &ret, sizeof(ret)))
				return -1;
			return ret;

		case TOUCH_IOCTL_SET_TOUCH_MODE:
			if (copy_from_user(&nval, argp, 2))
				return -1;
			ret = ts_write_touchmode((unsigned short)nval);
			return ret;

		case TOUCH_IOCTL_GET_REG:
			down(&g_misc_dev->work_procedure_lock);
			if (g_misc_dev->work_procedure != TS_NO_WORK) {
				printk("error: other process occupied.. (%d)\n",g_misc_dev->work_procedure);
				up(&g_misc_dev->work_procedure_lock);
				return -1;
			}

			g_misc_dev->work_procedure = TS_SET_MODE;

			if (copy_from_user(&reg_ioctl,
				argp, sizeof(struct _reg_ioctl))) {
				g_misc_dev->work_procedure = TS_NO_WORK;
				up(&g_misc_dev->work_procedure_lock);
				printk("error : copy_from_user\n");
				return -1;
			}

			val = 0;
			if (ts_read_data(g_misc_dev->client,reg_ioctl.addr, (unsigned char *)&val, 2) < 0)
				ret = -1;

			nval = (int)val;

			if (copy_to_user(reg_ioctl.val, (unsigned char *)&nval, sizeof(nval))) {
				g_misc_dev->work_procedure = TS_NO_WORK;
				up(&g_misc_dev->work_procedure_lock);
				printk("error : copy_to_user\n");
				return -1;
			}

			SSL_DEBUG("read : reg addr = 0x%x, val = 0x%x, ret = %d",reg_ioctl.addr, nval, ret);

			g_misc_dev->work_procedure = TS_NO_WORK;
			up(&g_misc_dev->work_procedure_lock);
			return ret;

		case TOUCH_IOCTL_SET_REG:
			down(&g_misc_dev->work_procedure_lock);
			if (g_misc_dev->work_procedure != TS_NO_WORK) {
				printk("error: other process occupied.. (%d)\n",g_misc_dev->work_procedure);
				up(&g_misc_dev->work_procedure_lock);
				return -1;
			}

			g_misc_dev->work_procedure = TS_SET_MODE;
			if (copy_from_user(&reg_ioctl, argp, sizeof(struct _reg_ioctl))) {
				g_misc_dev->work_procedure = TS_NO_WORK;
				up(&g_misc_dev->work_procedure_lock);
				printk("error : copy_from_user\n");
				return -1;
			}

			if (copy_from_user(&val, reg_ioctl.val, 2)) {
				g_misc_dev->work_procedure = TS_NO_WORK;
				up(&g_misc_dev->work_procedure_lock);
				printk("error : copy_from_user\n");
				return -1;
			}

			if (ts_write_data(g_misc_dev->client,reg_ioctl.addr, (unsigned char *)&val, 2) < 0)
				ret = -1;

			SSL_DEBUG("write : reg addr = 0x%x, val = 0x%x",reg_ioctl.addr, val);

			g_misc_dev->work_procedure = TS_NO_WORK;
			up(&g_misc_dev->work_procedure_lock);
			return ret;

		case TOUCH_IOCTL_GET_GRAPH_DATA:
			down(&g_misc_dev->work_procedure_lock);
			if (g_misc_dev->work_procedure != TS_NO_WORK) {
				printk("error: other process occupied.. (%d)\n",g_misc_dev->work_procedure);
				up(&g_misc_dev->work_procedure_lock);
				return -1;
			}

			g_misc_dev->work_procedure = TS_SET_MODE;

			if (copy_from_user(&reg_ioctl,argp, sizeof(struct _reg_ioctl))) {
				printk("error : copy_from_user\n");
				g_misc_dev->work_procedure = TS_NO_WORK;
				up(&g_misc_dev->work_procedure_lock);
				return -1;
			}

			val = 0;
			if (ts_read_data(g_misc_dev->client,reg_ioctl.addr, (unsigned char *)data, 64) < 0) {
				g_misc_dev->work_procedure = TS_NO_WORK;
				up(&g_misc_dev->work_procedure_lock);
				return -1;
			}

			if (copy_to_user(reg_ioctl.val, (unsigned char *)&data, 64)) {
				printk("error : copy_to_user\n");
				g_misc_dev->work_procedure = TS_NO_WORK;
				up(&g_misc_dev->work_procedure_lock);
				return -1;
			}

			SSL_DEBUG("read : reg addr = 0x%x", reg_ioctl.addr);
			g_misc_dev->work_procedure = TS_NO_WORK;
			up(&g_misc_dev->work_procedure_lock);
			return ret;

		case TOUCH_IOCTL_QUEUE_CLEAR:
			g_misc_dev->ftdata->queue_front = 0;
			g_misc_dev->ftdata->queue_rear = 0;
			SSL_DEBUG("Rawdata queue clear!!!\n");
			g_queue_count = 0;
			return ret;

		case TOUCH_IOCTL_RESET_TP:  
			down(&g_misc_dev->work_procedure_lock);

			g_misc_dev->work_procedure = TS_IN_UPGRADE;
			ssl_init(g_misc_dev);
			g_misc_dev->work_procedure = TS_NO_WORK;

			ssl_clear_int(g_ftdev->client);
			up(&g_misc_dev->work_procedure_lock);

			return ret;

		case TOUCH_IOCTL_RESET_TP1: 

			down(&g_misc_dev->work_procedure_lock);

			g_misc_dev->work_procedure = TS_IN_UPGRADE;
			ssl_init(g_misc_dev);
			g_misc_dev->work_procedure = TS_NO_WORK;
			g_power_status = LPM_RESUME;
			ssl_clear_int(g_ftdev->client);

			up(&g_misc_dev->work_procedure_lock);

			return ret;

		case TOUCH_IOCTL_DEBUG_TEST: 
			if (copy_from_user(&nval, argp, 1))
				return -1;
			SSL_DEBUG("the debug value got is %d\n", nval);
			return ret;

		case TOUCH_IOCTL_GET_RAW_DATA:
			if (g_misc_dev->touch_mode == TOUCH_POINT_MODE) {
				return -1;
			}

			if (copy_from_user(&raw_ioctl, argp, sizeof(raw_ioctl))) {
				printk("error : copy_from_user.\n");
				return -1;
			}

			if ((charData = get_front_queue_buff(g_misc_dev->ftdata)) == NULL) {
				printk("error: rawdata queue is empty.\n");
				return -3;
			}

			if (copy_to_user(raw_ioctl.buf,  charData, raw_ioctl.sz)) {
				printk("error : copy_to_user.\n");
				return -1;
			}
			get_queue(g_misc_dev->ftdata);

			return ret;

		case TOUCH_IOCTL_SET_DOWNLOAD:
			down(&g_misc_dev->work_procedure_lock);
			g_misc_dev->work_procedure = TS_IN_UPGRADE;

			memset(&down_ioctl, 0, sizeof(struct _down_ioctl));
			if (copy_from_user(&down_ioctl, argp, sizeof(struct _down_ioctl)))
			{
				g_misc_dev->work_procedure = TS_NO_WORK;
				ssl_clear_int(g_ftdev->client);
				printk("error : copy_from_user.\n");
				return -1;
			}

			if (copy_from_user(&data, down_ioctl.file, 256))
			{
				g_misc_dev->work_procedure = TS_NO_WORK;
				ssl_clear_int(g_ftdev->client);
				up(&g_misc_dev->work_procedure_lock);
				printk("error : copy_from_user.\n");
				return -1;
			}

			SSL_DEBUG("NAME : %s", down_ioctl.file);
			SSL_DEBUG("SIZE : %d", down_ioctl.sz);

			ret = dl_force_upgrade_by_file(down_ioctl.file);
			if (ret < 0) {
				printk("error: firmware update by file failed.\n");
			}

			ssl_init(g_misc_dev);
			g_misc_dev->work_procedure = TS_NO_WORK;

			ssl_clear_int(g_ftdev->client);
			up(&g_misc_dev->work_procedure_lock);

			return ret;
#if 0
		case TOUCH_IOCTL_DEBUG_TEST: 
			if (copy_from_user(&nval, argp, 1))
				return -1;
			SSL_DEBUG("the debug value got is %d", nval);
			if (nval == 2) {
				mt_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
			}
			else if (nval ==3) {
				mt_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
				ssl_clear_int(g_ftdev->client);
			}
			return ret;
#endif
		default:
			break;
	}

	return 0;
}

static ssize_t ts_misc_write(struct file* file, const char __user* buf, size_t count, loff_t* offset)
{
	unsigned char* kbuf;
	unsigned short reg;
	int ret=0;

	kbuf = kmalloc(count+1, GFP_KERNEL);
	if (!kbuf) return 0;

	if (copy_from_user(kbuf, buf, count))
	{
		printk("error: no enough memory!\n");
		goto ERROR;
	}

	reg = (unsigned short)(kbuf[3]<<8)| kbuf[2];

	SSL_DEBUG("ssd_misc_write, write addr[0x%x]--data = [0x%02x%02x ] bytes",reg, kbuf[4], kbuf[5]);

	ret=ts_write_data(g_misc_dev->client, (unsigned short)(kbuf[3]<<8)| kbuf[2], kbuf+4, kbuf[0]|(kbuf[1]<<8));

ERROR:
	kfree(kbuf);
	return ret;
}

static ssize_t ts_misc_read(struct file* file, char __user* buf, size_t count, loff_t* offset)
{
	char* kbuf;
	uint16_t reg;
	int ret = 0;

	kbuf = kmalloc(count, GFP_KERNEL);
	if (!kbuf) return 0;

	if (copy_from_user(kbuf, buf, 2))
	{
		printk("error: no enough memory!\n");
		goto ERROR;
	}

	reg = (((uint16_t) kbuf[1]) << 8) | kbuf[0];

	ret=ts_read_data(g_misc_dev->client, reg, kbuf, count);

	if (copy_to_user(buf, kbuf, count))
	{
		printk("error: no enough memory!\n");
		ret = 0;
	}
	SSL_DEBUG("ssd6600, misc read ret value = %d",ret);

ERROR:
	kfree(kbuf);
	return ret;
}

static const struct file_operations ts_misc_fops =
{
	.open = ts_misc_fops_open,
	.release = ts_misc_fops_release,
	.read = ts_misc_read,
	.write = ts_misc_write,
	.unlocked_ioctl = ts_misc_fops_ioctl,
	.unlocked_ioctl = ts_misc_fops_ioctl,
//#ifdef CONFIG_COMPAT
//	.compat_ioctl = ts_misc_fops_compat_ioctl,
//#endif
};

static struct miscdevice touch_g_misc_dev =
{
	.minor = MISC_DYNAMIC_MINOR,
	.name = "sentron_touch_misc",
	.fops = &ts_misc_fops,
};

//************************************************************
//ssd614x command function
//************************************************************

int ssl_get_usr_verion(void)
{
	int err = 0;
	unsigned short val = 0x0000;

	err = ts_read_data(g_misc_client, SSL_USR_VERSION, (unsigned char *)&(val), 2);
	if (err < 0)
	{
		printk("error : read SSL_USR_VERSION.\n");
		return 0xFFFF;
	}

	return val;
}

int ssl_get_chip_id(void)
{
	int err = 0;
	unsigned char buff[2];
	unsigned short val = 0x0000;

	err = ts_read_data(g_misc_client, SSL_CHIP_ID, buff, 2);
	if (err < 0)
	{
		printk("error : read SSL_CHIP_ID.\n");
		return -EAGAIN;
	}

	val = (buff[1]&0xFF)<<8|(buff[0]&0xFF);

	SSL_DEBUG("#################################");
	SSL_DEBUG("SSL_CHIP_ID : 0x%04x", val);

	return val;
}



int ssl_reset(void)
{
	int retry = 100;

	SSL_DEBUG("ssl_reset");

	msleep(10);
	gpio_set_value(g_misc_dev->reset_pin, 1);
	msleep(10);
	gpio_set_value(g_misc_dev->reset_pin, 0);
	msleep(5);
	gpio_set_value(g_misc_dev->reset_pin, 1);
	msleep(20);

	/* wait DS init; */
	if (g_misc_dev != NULL)
		int_pin_check(g_misc_dev, retry * 2);

	ssl_get_chip_id();

	return 0;
}

static int ssl_hw_init(void)
{
	struct device_node *node = NULL;
	int ret = 0;
	int ints[2] = { 0, 0 };
	int ints1[2] = { 0, 0 };

	node = of_find_matching_node(node, touch_of_match);
	if (node) {
		of_property_read_u32_array(node, "debounce", ints, ARRAY_SIZE(ints));
		gpio_set_debounce(ints[0], ints[1]);

		of_property_read_u32_array(node, "interrupts", ints1, ARRAY_SIZE(ints1));
		tpd_int_gpio_number = ints1[0];
		tpd_intr_type = ints1[1];

		touch_irq = irq_of_parse_and_map(node, 0);

		TPD_DMESG("Device tpd_intr_type = %d!\n", tpd_intr_type);
		TPD_DMESG("tpd_intr_type = %d!IRQF_TRIGGER_LOW\n", tpd_intr_type);
	} else {
		TPD_DMESG("tpd request_irq can not find touch eint device node!.\n");
		ret = -1;
	}
	TPD_DMESG("[%s]irq:%d, debounce:%d-%d:\n", __func__, touch_irq, ints[0], ints[1]);
	
	ret = gpio_request(P_GPIO_CTP_RST_PIN, "tpd_rst");
	if (ret)
		TPD_DMESG("[ssd2098] ssl_hw_init : gpio_request (%d)fail\n", P_GPIO_CTP_RST_PIN);

	ret = gpio_direction_output(P_GPIO_CTP_RST_PIN, 0);
	if (ret)
		TPD_DMESG("[ssd2098] ssl_hw_init : gpio_direction_output (%d)fail\n",
		       P_GPIO_CTP_RST_PIN);

	GTP_GPIO_OUTPUT(GTP_RST_PORT, 0);
	tpd_rst_gpio_number = P_GPIO_CTP_RST_PIN;

	TPD_DMESG("tpd_rst_gpio_number %d\n", tpd_rst_gpio_number);
	TPD_DMESG("tpd_int_gpio_number %d\n", tpd_int_gpio_number);
	return 0;
}

static void ssl_hw_deint(void)
{
	if (gpio_is_valid(g_misc_dev->reset_pin))
		gpio_free(g_misc_dev->reset_pin);
	if (gpio_is_valid(g_misc_dev->int_pin))
		gpio_free(g_misc_dev->int_pin);
}
int ssl_init(struct ssl_device *g_ftdev)
{
	int err = 0;
	unsigned short val = 0x0000;

	struct i2c_client *client = g_ftdev->client;
	struct ssl_config *ftconfig = g_ftdev->ftconfig;

	//reset chip;
	ssl_reset();

	// write config info
	val = MAX_POINT_BUFFER_SIZE;
	err = ts_write_data(client, SSL_POINT_SCALE, (unsigned char *)&(val), 2);
	if (err < 0)
	{
		err = -EAGAIN;
		printk("error : write point scale.\n");
	}

	val = TOUCH_POINT_MODE;
	err = ts_write_data(client, SSL_TOUCH_MODE, (unsigned char *)&(val), 2);
	if (err < 0)
	{
		err = -EAGAIN;
		printk("error : write touch point mode.\n");
	}

	val = SSL_X_MAX;
	err = ts_write_data(client, SSL_X_RESOLUTION, (unsigned char *)&(val), 2);
	if (err < 0)
	{
		printk("error : write x resolution.\n");
		return -EAGAIN;
	}

	val = SSL_Y_MAX;
	err = ts_write_data(client, SSL_Y_RESOLUTION, (unsigned char *)&(val), 2);
	if (err < 0)
	{
		printk("error : write y resolution.\n");
		return -EAGAIN;
	}

	val = SSL_MAX_POINT;
	err = ts_write_data(client, SSL_USING_POINT_NUM, (unsigned char *)&(val), 2);
	if (err < 0)
	{
		printk("error : write point num.\n");
		return -EAGAIN;
	}

	// read config info
	err = ts_read_data(client, SSL_FW_VERSION, (unsigned char *)&(ftconfig->fw_ver), 2);
	if (err < 0)
	{
		printk("error : read x node.\n");
		return -EAGAIN;
	}
	err = ts_read_data(client, SSL_USR_VERSION, (unsigned char *)&(ftconfig->data_ver), 2);
	if (err < 0)
	{
		printk("error : read x node.\n");
		return -EAGAIN;
	}

	err = ts_read_data(client, SSL_TOTAL_X_NODE, (unsigned char *)&(ftconfig->x_node), 2);
	if (err < 0)
	{
		printk("error : read x node.\n");
		return -EAGAIN;
	}

	err = ts_read_data(client, SSL_TOTAL_Y_NODE, (unsigned char *)&(ftconfig->y_node), 2);
	if (err < 0)
	{
		printk("error : read y node.\n");
		return -EAGAIN;
	}

	err = ts_read_data(client, SSL_X_RESOLUTION, (unsigned char *)&(ftconfig->max_x), 2);
	if (err < 0)
	{
		err = -EAGAIN;
		printk("error : read x resolution.\n");
	}

	err = ts_read_data(client, SSL_Y_RESOLUTION, (unsigned char *)&(ftconfig->max_y), 2);
	if (err < 0)
	{
		err = -EAGAIN;
		printk("error : read y resolution.\n");
	}

	err = ts_read_data(client, SSL_USING_POINT_NUM, (unsigned char *)&(ftconfig->using_point), 2);
	if (err < 0)
	{
		err = -EAGAIN;
		printk("error : read using point.\n");
	}

	SSL_DEBUG("#################################");
	SSL_DEBUG(" fw version : %d", ftconfig->fw_ver);
	SSL_DEBUG("usr version : %d", ftconfig->data_ver);
	SSL_DEBUG(" resolution : %d X %d", ftconfig->max_x, ftconfig->max_y);
	SSL_DEBUG("  node info : %d X %d", ftconfig->x_node, ftconfig->y_node);
	SSL_DEBUG("using point : %d", ftconfig->using_point);
	SSL_DEBUG("#################################");

	return err;
}

#ifdef SUPPORT_KEY_BUTTON
/*
	send android key event to OS
*/
static int ssl_send_key(struct input_dev *input, unsigned int key, int level)
{
	input_report_key(input, key, level);
	input_sync(input);

	return 0;
}

static inline int ssl_read_keydata(struct i2c_client *client, unsigned char *keydata)
{
	int err = 0;

	err = ts_read_data(client, SSL_GET_KEYDATA, (unsigned char *)(keydata), 2);
	if (err < 0) {
		printk("error : read key data.\n");
		return -EAGAIN;
	}

	SSL_DEBUG("read key data : 0x%02x%02x", keydata[1], keydata[0]);

	return err;
}

static int ssl_read_keydata_android(void)
{
	int err = 0;
	struct input_dev *input = NULL;
	unsigned int gesture_key = 0x00;
	struct ssl_data *data = NULL;
	unsigned short keydata = 0x00;

	err = ssl_read_keydata(g_ftdev->client, (unsigned char *)(&keydata));
	if (err < 0) {
		printk("error: failed read keydata!!\n");
		return err;
	}

	data = g_ftdev->ftdata;
	input = tpd->dev;

	// key down event
	if ((keydata&STR_KEYDATA_MENU_DOWN) == STR_KEYDATA_MENU_DOWN) {
		gesture_key = KEY_MENU;
		data->keydata |= STR_KEYDATA_MENU_DOWN;
		ssl_send_key(input, gesture_key, 1);
	}
	if ((keydata&STR_KEYDATA_HOME_DOWN) == STR_KEYDATA_HOME_DOWN) {
		gesture_key = KEY_HOMEPAGE;
		data->keydata |= STR_KEYDATA_HOME_DOWN;
		ssl_send_key(input, gesture_key, 1);
	}
	if ((keydata&STR_KEYDATA_BACK_DOWN) == STR_KEYDATA_BACK_DOWN) {
		gesture_key = KEY_BACK;
		data->keydata |= STR_KEYDATA_BACK_DOWN;
		ssl_send_key(input, gesture_key, 1);
	}
	// key up event
	if ((keydata&STR_KEYDATA_MENU_UP) == STR_KEYDATA_MENU_UP) {
		if ((data->keydata&(STR_KEYDATA_MENU_DOWN)) == 0) goto ERROR;
		gesture_key = KEY_MENU;
		data->keydata &= ~(STR_KEYDATA_MENU_DOWN);
		ssl_send_key(input, gesture_key, 0);
	}
	if ((keydata&STR_KEYDATA_HOME_UP) == STR_KEYDATA_HOME_UP) {
		if ((data->keydata&(STR_KEYDATA_HOME_DOWN)) == 0) goto ERROR;
		gesture_key = KEY_HOMEPAGE;
		data->keydata &= ~(STR_KEYDATA_HOME_DOWN);
		ssl_send_key(input, gesture_key, 0);
	}
	if ((keydata&STR_KEYDATA_BACK_UP) == STR_KEYDATA_BACK_UP) {
		if ((data->keydata&(STR_KEYDATA_BACK_DOWN)) == 0) goto ERROR;
		gesture_key = KEY_BACK;
		data->keydata &= ~(STR_KEYDATA_BACK_DOWN);
		ssl_send_key(input, gesture_key, 0);
	}
	if (gesture_key == 0x00) {
		printk("error: Not supported key value : 0x%04x.\n", keydata);
		return -1;
	}

	SSL_DEBUG("android key : 0x%08x", gesture_key);

	return err;
ERROR:
	SSL_DEBUG("detect up key without down key : 0x%04x", keydata);
	return -2;
}
#endif

#ifdef SUPPORT_GESTURE
static ssize_t str_gesture_android(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	input_report_key(g_ftdev->input_dev, KEY_BACK, 1);
	input_sync(g_ftdev->input_dev);

	return count;
}
static DEVICE_ATTR(gesture, S_IRUGO, NULL, str_gesture_android);

static inline int ssl_read_gesture(struct i2c_client *client, unsigned char *type, unsigned char *gesture)
{
	int err = 0;
	unsigned short data = 0;

	err = ts_read_data(client, SSL_GET_GESTURE, (unsigned char *)(&data), 2);
	if (err < 0)
	{
		printk("error : read Gesture.\n");
		return -EAGAIN;
	}

	SSL_DEBUG("read gesture : 0x%04x", data);
	*type = (data&0xff00) >> 8;
	*gesture = data&0x00ff;

	return err;
}

static int ssl_read_gesture_android(void)
{
	int err = 0;
//	unsigned int gesture_key = 0x00;
	unsigned char type, gesture;

	err =ssl_read_gesture(g_ftdev->client, &type, &gesture);
	if (err > 0)
	{
		if ((type&GESTURE_STATUS_KNOCK_ALL) > 0)
		{
#if 1

			input_report_key(tpd->dev, KEY_BACK, 1);
			input_sync(tpd->dev);
#else
			gesture_key = KEY_BACK;		// for wake up
			ssl_send_key(g_ftdev->input_dev, gesture_key, 1);
			ssl_send_key(g_ftdev->input_dev, gesture_key, 0);
#endif
		}
	}
	return err;
}
#endif

#ifdef SUPPORT_AUX
static int ssl_read_aux_android(void)
{
	int err = 0;
	unsigned short aux = 0x01;

	err = ts_read_data(g_ftdev->client, SSL_AUX, (unsigned char *)(&aux), 2);
	if (err > 0)
	{
		printk("error : read SSL_AUX.\n");
		return -EAGAIN;
	}

	SSL_DEBUG("Read AUX info : 0x%04x", aux);
	return err;

}
#endif

/*
	send android touch pointers event to OS
*/
static int ssl_touch_down_up(int i, int xpos, int ypos, int width, int isdown)
{
	if (isdown)
	{
		SSL_DEBUG("pos[%d]: X = %d , Y = %d, W = %d", i, xpos, ypos, width);
		input_report_key(tpd->dev, BTN_TOUCH, 1);
		input_report_abs(tpd->dev, ABS_MT_TRACKING_ID, i); 
		input_report_abs(tpd->dev, ABS_MT_TOUCH_MAJOR, 1);
		input_report_abs(tpd->dev, ABS_MT_PRESSURE, width);
		input_report_abs(tpd->dev, ABS_MT_WIDTH_MAJOR,width);
		input_report_abs(tpd->dev, ABS_MT_POSITION_X, xpos);
		input_report_abs(tpd->dev, ABS_MT_POSITION_Y, ypos);
		input_mt_sync(tpd->dev);

	}
	else
	{
		SSL_DEBUG("pos[%d]: X = %d , Y = %d, W = %d", i, xpos, ypos, width);
		input_report_key(tpd->dev, BTN_TOUCH, 0);
		input_mt_sync(tpd->dev);
	}
	return 0;
}

static int ssl_report_release(struct ssl_device *ftdev)
{
	int i = 0;

	for (i = 0; i < SSL_MAX_POINT; i++) {
		ssl_touch_down_up(i, 0, 0, 0, 0);
	}
	input_sync(tpd->dev);

	return 0;
}


/*
	send a touch point to AP
*/
static int ssl_report(void)
{
	int i = 0;
	unsigned char status, length, id, w;
	unsigned short	x, y;

	struct ssl_data *data = g_ftdev->ftdata;

	status = (data->point_info & 0xff00) >> 8;
	length = (data->point_info & 0x00ff);

	if (length == 0)
	{
		ssl_report_release(g_misc_dev);
	}
	else
	{
		for (i = 0; i < length/2; i++)
		{
			id = (data->points[i].id_x  & 0xf000) >> 12;
			x = (data->points[i].id_x  & 0x0fff);
			w = (data->points[i].w_y  & 0xf000) >> 12;
			y = (data->points[i].w_y  & 0x0fff);
			if(w)
			{
				ssl_touch_down_up(id, x, y, w, 1);
				m_up_event[id] = 1;
			}
			else
			{
				if (m_up_event[id] == 1) {
					printk("[ID:%d] UP event", id);
					ssl_touch_down_up(id, x, y, w, 0);
				} else {
					printk("[ID:%d]unknown UP", id);
				}
			}
		}
	}
	input_mt_report_pointer_emulation(tpd->dev, true);
	input_sync(tpd->dev);
	return 0;
}

static int ssl_read_rawdata(void)
{
	int err = 0;
	unsigned int total_node;
	int sz=0;
	char *buffer, *buffer2;
	int len = MAX_RAWDATA_BUFFER_SIZE;
	int max = 10;
	char temp[MAX_RAWDATA_BUFFER_SIZE];
	unsigned char status=0x00, length=0x00;

	struct ssl_data *data = g_ftdev->ftdata;

//	if (gpio_get_value(g_ftdev->int_pin)) {
//		/*interrupt pin is high, not valid data.*/
//		printk("error: read rawdata... inturrpt pin is high");
//		return 0;
//	}
	gpio_direction_input(tpd_int_gpio_number);
	if (gpio_get_value(g_ftdev->int_pin)) {
		/*interrupt pin is high, not valid data.*/
		SSL_DEBUG("read rawdata... inturrpt pin is high");
		return 0;
	}

	if (down_trylock(&g_ftdev->raw_data_lock))
	{
		printk("error: fail to occupy sema(%s).\n", __func__);
		goto EEROR;
	}

	err = ts_read_data(g_ftdev->client,SSL_STATUS_LENGTH, (unsigned char *)(&data->point_info), 2);
	if (err < 0)
	{
		printk("error: read point info using i2c.\n");
		goto EEROR;
	}

	buffer = get_rear_queue_buff(data);
	if (buffer == NULL)
	{
		printk("error: rawdata queue is full.\n");
		goto EEROR;
	}

	if (data->point_info > 0 && ((data->point_info&0x00ff) < 0x00ff))
	{
		status = (data->point_info & 0xff00) >> 8;
		length = (data->point_info & 0x00ff) << 1;
		if (length > 0) length += 2;	// status & length
	}
	buffer2 = buffer;
	total_node = g_ftdev->ftconfig->x_node * g_ftdev->ftconfig->y_node;
	sz = total_node*2 + length;

	while(sz > 0)
	{
		if (sz <= MAX_RAWDATA_BUFFER_SIZE) len = sz;

		sz -= MAX_RAWDATA_BUFFER_SIZE;
		err = ts_read_data(g_ftdev->client, SSL_RAW_DATA, temp, len);
		if (err < 0)
		{
			printk("error : read raw data.\n");
			goto EEROR;
		}

		memcpy(buffer, temp, len);
		buffer = buffer+len;

		if (--max < 0) 
		{
			err = -EAGAIN; 
			break;
		}
	}

	if (length > 0)
	{
		memcpy((unsigned char *)&data->point_info, &buffer2[total_node*2], length);
		ssl_report();
	}

	put_queue(data);

	g_ftdev->update = 1;

EEROR:
	udelay(DELAY_FOR_SIGNAL_DELAY);
	up(&g_ftdev->raw_data_lock);

	return err;
}

static int ssl_read_points(void)
{
	int err = 0;
	unsigned char status, length;
	struct ssl_data *data = g_ftdev->ftdata;

	SSL_DEBUG("ssl_read_points");

//	if (gpio_get_value(g_ftdev->int_pin)) {
//		/*interrupt pin is high, not valid data.*/
//		printk("error: read points... interrupt pin is high");
//		return -EAGAIN;
//	}
	gpio_direction_input(tpd_int_gpio_number);
	if (gpio_get_value(g_ftdev->int_pin)) {
		/*interrupt pin is high, not valid data.*/
		SSL_DEBUG("read points... interrupt pin is high");
		return -EAGAIN;
	}

	err = ts_read_data(g_ftdev->client, SSL_STATUS_LENGTH, (unsigned char *)(&data->point_info), 2);
	if (err < 0)
	{
		printk("error: read point info using i2c.\n");
		goto EEROR;
	}

	if (data->point_info == 0x0000)
	{
		ssl_report();
		goto EEROR;
	}

	status = (data->point_info & 0xff00) >> 8;
	length = (data->point_info & 0x00ff) << 1;

	SSL_DEBUG("status : 0x%02X, %d", status, length);

// Gesture check
#ifdef SUPPORT_GESTURE
	if ((status&STATUS_CHECK_PALM_GESTURE) == STATUS_CHECK_PALM_GESTURE)
	{
		SSL_DEBUG("SUPPORT_GESTURE");
		ssl_read_gesture_android();
	}
#endif

// AUX Report
#ifdef SUPPORT_AUX
	if ((status&STATUS_CHECK_AUX) == STATUS_CHECK_AUX)
	{
		SSL_DEBUG("SUPPORT_AUX");
		ssl_read_aux_android();
	}
#endif

// TouchKey Report
#ifdef SUPPORT_KEY_BUTTON
	if ((status&STATUS_CHECK_KEY) == STATUS_CHECK_KEY)
	{
		SSL_DEBUG("SUPPORT_KEY_BUTTON");
		ssl_read_keydata_android();
	}
#endif

// Pointer Report
	SSL_DEBUG("Pointer Report");
	if (length > 0 && ((status&0x0F) > 0))
	{
		SSL_DEBUG("Pointer Report 1");
		err = ts_read_data(g_ftdev->client, SSL_POINT_DATA, (unsigned char *)(&data->points), length);
		if (err < 0)
		{
			printk("error: read point info using i2c.\n");
			goto EEROR;
		}
		ssl_report();
	}

EEROR:
	udelay(DELAY_FOR_SIGNAL_DELAY);

	return err;
}

static void ssl_ts_work(struct work_struct *work)
{
	SSL_DEBUG("SSL_WORK(%d)",g_ftdev->touch_mode);

#ifdef SUPPORT_GESTURE 
	if ((g_power_status&LPM_SUSPEND_DELAY) == LPM_SUSPEND_DELAY)
	{
		goto EEROR;
	}
#endif

	if (down_trylock(&g_ftdev->work_procedure_lock))
	{
		printk("error: fail to occupy sema.\n");
		do
		{
			udelay(10);
		} while(down_trylock(&g_ftdev->work_procedure_lock));
		ssl_clear_int(g_ftdev->client);
		udelay(DELAY_FOR_SIGNAL_DELAY);
		goto EEROR;
	}

	if (g_ftdev->work_procedure != TS_NO_WORK)
	{
		printk("error: other process occupied.\n");
		udelay(DELAY_FOR_SIGNAL_DELAY);
		ssl_clear_int(g_ftdev->client);
		goto EEROR;
	}

	g_ftdev->work_procedure = TS_NORMAL_WORK;
	if (g_ftdev->touch_mode != TOUCH_POINT_MODE)
	{
		ssl_read_rawdata();
		ssl_clear_int(g_ftdev->client);
	}
	else
	{
		ssl_read_points();
		ssl_clear_int(g_ftdev->client);
	}

EEROR:
	if (g_ftdev->work_procedure == TS_NORMAL_WORK)
	{
		g_ftdev->work_procedure = TS_NO_WORK;
	}

	up(&g_ftdev->work_procedure_lock);
	GTP_GPIO_AS_INT(GTP_INT_PORT);
}

static irqreturn_t ssl_interrupt(int irq, void *devid)
{
	struct ssl_device *ftdevice = (struct ssl_device *)devid;
	ssl_ts_work(&ftdevice->work);
	return IRQ_HANDLED;
}

static struct attribute *str_attrs[] = {
	&dev_attr_version.attr,
	&dev_attr_ssdtouch.attr,
#ifdef SUPPORT_GESTURE
	&dev_attr_gesture.attr,
#endif
	NULL,
};

static const struct attribute_group str_attr_group =
{
	.attrs = str_attrs,
};

static int tpd_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct ssl_data *ftdata = NULL;
	struct ssl_config *ftconfig = NULL;
	struct workqueue_struct *wq = NULL;
	int err = 0;
	//int i=0;

	SSL_DEBUG("########################################");
	SSL_DEBUG("#### TOUCH DRIVER PROBE 1 ####");

	//1.power 3.3 and 1.8

	//2.malloc data size
	g_ftdev = (struct ssl_device *)kzalloc(sizeof(struct ssl_device), GFP_KERNEL);
	if (!g_ftdev)
	{
		printk("error: Create ssl device failed.\n");
		err = -ENOMEM;
		goto create_ssl_failed;
	}

	ftdata = (struct ssl_data *)kzalloc(sizeof(struct ssl_data), GFP_KERNEL);
	if (!ftdata)
	{
		printk("error: Create ssl data failed.\n");
		err = -ENOMEM;
		goto create_data_failed;
	}

	// initial rawdata queue
	ftdata->queue_front = 0;
	ftdata->queue_rear = 0;
	ftdata->keydata = 0x00;

	ftconfig = (struct ssl_config *)kzalloc(sizeof(struct ssl_config), GFP_KERNEL);
	if (!ftconfig)
	{
		printk("error: Create ssl config failed.\n");
		err = -ENOMEM;
		goto create_config_failed;
	}

	wq = create_singlethread_workqueue("ssl_touch");

	if (!wq) {
		printk("Create workqueue failed");
		goto create_workqueue_failed;
	}

	g_ftdev->touch_mode = TOUCH_POINT_MODE;
	g_misc_dev = g_ftdev;

	g_ftdev->workqueue = wq;
	g_ftdev->client = client;
	g_ftdev->ftconfig = ftconfig;
	g_ftdev->ftdata = ftdata;

	/* for I2C function */
	g_ftdev->i2c_func.ts_read_data = ts_read_data;
//	g_ftdev->i2c_func.ts_read_data_ex = ts_read_data_ex;
	g_ftdev->i2c_func.ts_write_data = ts_write_data;

	g_misc_client= client;
	err = regulator_enable(tpd->reg);
	if (err != 0)
		TPD_DMESG("Failed to enable reg-vgp6: %d\n", err);	
	
	//mutex_init(&g_ftdev->lock);
	
	INIT_WORK(&g_ftdev->work, ssl_ts_work);
	
	// gpio setting
	ssl_hw_init();
	g_ftdev->reset_pin = tpd_rst_gpio_number;
	g_ftdev->irq = touch_irq;
	g_ftdev->int_pin = tpd_int_gpio_number;
	
	SSL_DEBUG("#### SSD TOUCH DRIVER PROBE 2 ####");

	// reset ic and read ic info
	err = ssl_init(g_ftdev);
	if (err < 0)
	{
		printk("error: init failed.\n");
	}

	// check and upgrade
	dl_auto_update_init();

	// reset ic and read ic info
	err = ssl_init(g_ftdev);
	if (err < 0)
	{
		printk("error: init failed.\n");
	}
	SSL_DEBUG("#### SSD TOUCH DRIVER PROBE 3 ####");

	g_ftdev->work_procedure = TS_NO_WORK;
	sema_init(&g_ftdev->work_procedure_lock, 1);
	device_enable_async_suspend(&client->dev);
	
	GTP_GPIO_AS_INT(GTP_INT_PORT);
		err = request_threaded_irq(g_ftdev->irq, NULL,
			ssl_interrupt,
			IRQF_DISABLED | IRQF_ONESHOT | IRQF_TRIGGER_FALLING,
			"ssd614x", g_ftdev);
	if (err) {
		printk("Irq request failed");
		goto irq_request_failed;
	}

	sema_init(&g_ftdev->raw_data_lock, 1);
	err = misc_register(&touch_g_misc_dev);
	if (err)
	{
		printk("error: failed to register touch misc device.\n");
	}

	if (sysfs_create_group(&client->dev.kobj, &str_attr_group))
	{
		printk("error: failed to create sysfs group.\n");
	}

	tpd_load_status = 1;

	SSL_DEBUG("#### TOUCH DRIVER PROBE OK ####");
	SSL_DEBUG("########################################");
	return 0;

irq_request_failed:
	destroy_workqueue(wq);
	ssl_hw_deint();
create_workqueue_failed:
	kfree(ftconfig);
create_config_failed:
	kfree(ftdata);
create_data_failed:
	kfree(g_ftdev);
	g_misc_dev = NULL;
create_ssl_failed:
	return err;
}

static int tpd_remove(struct i2c_client *client)
{
	struct ssl_device *g_ftdev = i2c_get_clientdata(client);

	if (g_ftdev)
	{
		down(&g_ftdev->work_procedure_lock);

		g_ftdev->work_procedure = TS_REMOVE_WORK;
		ssl_hw_deint();
		free_irq(g_ftdev->irq, g_ftdev);
		misc_deregister(&touch_g_misc_dev);
		kfree(g_ftdev->ftconfig);
		kfree(g_ftdev->ftdata);
		kfree(g_ftdev);
		dl_auto_update_exit();
		up(&g_ftdev->work_procedure_lock);
	}

	i2c_set_clientdata(client, NULL);

	return 0;
}


static void tpd_suspend(struct device *dev)
{
	int ret = -1;
#if !defined(SUPPORT_POWERONOFF_SUSPEND_RESUME)
	unsigned short powermode = 0x0000;
	unsigned short retry_count = 0;
#endif
	SSL_DEBUG("tpd_suspend");
	disable_irq(touch_irq);
	flush_workqueue(g_ftdev->workqueue);
	ssl_report_release(g_misc_dev);
#ifdef SUPPORT_POWERONOFF_SUSPEND_RESUME
	gpio_set_value(g_misc_dev->reset_pin, 0);
	//GTP_GPIO_OUTPUT(GTP_RST_PORT, 0);
	ret = regulator_disable(tpd->reg);	/* disable regulator */
	if (ret)
		SSL_DEBUG("regulator_disable() failed!\n");
#else		
	powermode = 0x0200;
	ts_write_data(g_misc_dev->client,SSL_ADD_POWERMODE, (unsigned char *)&(powermode), 2);
	msleep(10);
	powermode = 0;
	ret = ts_read_data(g_misc_dev->client, SSL_ADD_POWERMODE, (unsigned char *)&(powermode), 2);
	if (ret < 0) {
		SSL_DEBUG("error: read SSL_ADD_POWERMODE error.\n");
		powermode = 0;
	}
	for(retry_count = 0; retry_count<10;retry_count++)
	{
		if(powermode != 0x0200)
		{
			powermode = 0x0200;
			ts_write_data(g_misc_dev->client,SSL_ADD_POWERMODE, (unsigned char *)&(powermode), 2);
			msleep(10);
			powermode = 0;
			ret = ts_read_data(g_misc_dev->client, SSL_ADD_POWERMODE, (unsigned char *)&(powermode), 2);
			if (ret < 0) {
				SSL_DEBUG("error: read SSL_ADD_POWERMODE error.\n");
				powermode = 0;
			}
		}
		else
		{
			SSL_DEBUG("tpd_suspend retry finished %d\n",retry_count);
			break;
		}
	}
#endif
}

static void tpd_resume(struct device *dev)
{
	int ret = -1;
#if !defined(SUPPORT_POWERONOFF_SUSPEND_RESUME)
	unsigned short powermode = 0x0000;
	unsigned short retry_count = 0;
#endif
	SSL_DEBUG("tpd_resume");
	enable_irq(touch_irq);
#ifdef SUPPORT_POWERONOFF_SUSPEND_RESUME
	ret = regulator_enable(tpd->reg);	/* enable regulator */
	if (ret)
		SSL_DEBUG("regulator_enable() failed!\n");	
	//reset ic
	ssl_init(g_ftdev);
#else
	powermode = 0x0100;
	ts_write_data(g_misc_dev->client,SSL_ADD_POWERMODE, (unsigned char *)&(powermode), 2);
	msleep(10);
	powermode = 0;
	ret = ts_read_data(g_misc_dev->client, SSL_ADD_POWERMODE, (unsigned char *)&(powermode), 2);
	if (ret < 0) {
		SSL_DEBUG("error: read SSL_ADD_POWERMODE error.\n");
		powermode = 0;
	}
	for(retry_count = 0; retry_count<10;retry_count++)
	{
		if(powermode != 0x0100)
		{
			powermode = 0x0100;
			ts_write_data(g_misc_dev->client,SSL_ADD_POWERMODE, (unsigned char *)&(powermode), 2);
			msleep(10);
			powermode = 0;
			ret = ts_read_data(g_misc_dev->client, SSL_ADD_POWERMODE, (unsigned char *)&(powermode), 2);
			if (ret < 0) {
				SSL_DEBUG("error: read SSL_ADD_POWERMODE error.\n");
				powermode = 0;
			}
		}
		else
		{
			SSL_DEBUG("tpd_resume retry finished %d\n",retry_count);
			break;
		}
	}
	//clear interrupt
	ssl_clear_int(g_ftdev->client);
#endif
}

static int tpd_detect(struct i2c_client* client, struct i2c_board_info* info)
{
	strcpy(info->type, TPD_DEVICE);
	return 0;
}

MODULE_DEVICE_TABLE(of, ssd6141_match_table);

static struct i2c_driver ssl_driver =
{
	.driver =
	{
		.of_match_table = of_match_ptr(ssd6141_match_table),
		.owner = THIS_MODULE,		
		.name = "ssd614x",
	},
	.probe = tpd_probe,
	.remove = tpd_remove,
	.id_table = tpd_ssl_id,
	.detect = tpd_detect,
};

static int tpd_local_init(void)
{
	int ret = 0;
	printk("Solomon SSD614X I2C Touchscreen Driver\n");

	tpd->reg = regulator_get(tpd->tpd_dev, "vtouch");	
	if (IS_ERR(tpd->reg))		
		printk("regulator_get() failed!\n");
	
	ret = regulator_set_voltage(tpd->reg, 2800000, 2800000);/* set 2.8v */
	if (ret)
		printk("regulator_set_voltage() failed!\n");
	ret = regulator_enable(tpd->reg);	/* enable regulator */
	if (ret)
		printk("regulator_enable() failed!\n");

	if (i2c_add_driver(&ssl_driver) != 0)
	{
		printk("error: SSD614X unable to add i2c driver.\n");
		return -1;
	}

	if (tpd_load_status == 0)
	{
		printk("error: SSD614X add error touch panel driver.\n");
		i2c_del_driver(&ssl_driver);
		return -1;
	}

#ifdef TPD_HAVE_BUTTON
	if (tpd_dts_data.use_tpd_button) {
		tpd_button_setting(tpd_dts_data.tpd_key_num, tpd_dts_data.tpd_key_local,
		tpd_dts_data.tpd_key_dim_local);
	}
#endif

	tpd_type_cap = 1;

	return 0;
}

static struct tpd_driver_t tpd_device_driver =
{
	.tpd_device_name = "ssd614x",
	.tpd_local_init = tpd_local_init,
	.suspend = tpd_suspend,
	.resume = tpd_resume,
#ifdef TPD_HAVE_BUTTON
	.tpd_have_button = 1,
#else
	.tpd_have_button = 0,
#endif

};

static int __init touch_ssl_init(void)
{
	tpd_get_dts_info();
	if (tpd_driver_add(&tpd_device_driver) < 0)
	{
		printk("error: add SSD614X driver failed.\n");
	}

	SSL_DEBUG("SSD614X touch panel driver init");

	return 0;
}

static void __exit touch_ssl_exit(void)
{
	SSL_DEBUG("SSD614X touch panel driver exit");

	tpd_driver_remove(&tpd_device_driver);
}

module_init(touch_ssl_init);
module_exit(touch_ssl_exit);

MODULE_LICENSE("GPL");

MODULE_AUTHOR("Solomon Systech (ShenZhen) Limited - binkazhang@solomon-systech.com");
MODULE_DESCRIPTION("ssd614x Touchscreen Driver 1.0");
MODULE_LICENSE("GPL v2");
