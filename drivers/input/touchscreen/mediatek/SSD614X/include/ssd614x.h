/*
	Solomon SSD614X Touch device driver

* Must have Change Macro
	1) SSD614X reset pin
		#define SSL_RESET_PIN	[GPIO]
	2) SSD614X Interrupt pin
		#define SSL_INT_PIN		[GPIO]
		#define SSL_IRQ			[IRQ]

* Changes depend on panel
	#define SSL_X_MAX		[resolution x]
	#define SSL_Y_MAX		[resolution y]
	#define SSL_MAX_X_NODE	[number of NODE X]
	#define SSL_MAX_Y_NODE	[number of NODE Y]

* Changes depend on customer
	#define SSL_MAX_POINT	[number of fingers]
*/
#ifndef __SSD614X_H
#define __SSD614X_H

#define GTP_GPIO_AS_INT(pin) tpd_gpio_as_int(pin)
#define GTP_GPIO_OUTPUT(pin, level) tpd_gpio_output(pin, level)

#define P_GPIO_CTP_EINT_PIN       10//65
#define P_GPIO_CTP_RST_PIN        62//66

//I2C max transaction length for TPV platform
#define MAX_TRANSACTION_LENGTH        8
#define I2C_MASTER_CLOCK              300
#define SSL_ADDR_LENGTH             2
#define MAX_I2C_TRANSFER_SIZE         (MAX_TRANSACTION_LENGTH - SSL_ADDR_LENGTH)

//###############################################################################
// Support fucntion
//###############################################################################
//#define SUPPORT_POWERONOFF_SUSPEND_RESUME				  //Turn off power supply during suspend
#define SUPPORT_GESTURE					// for support of LPM
//#define SUPPORT_AUX					// for support of AUX bit of SnL
//#define SUPPORT_KEY_BUTTON				// for key H/W button
#define SUPPORT_I2C_READ_8BYTE

#define TOUCH_POINT_MODE			0
#define RAW_DATA_MODE				1
#define MAX_RAWDATA_BUFFER_SIZE		512
#define MAX_POINT_BUFFER_SIZE		8

#define SSL_I2C_NAME			"ssd614x"					// I2C device name
#define SSL_I2C_ADDR			0x48						    // I2C slave address
//###############################################################################
//#define CONFIG_TOUCHSCREEN_SSL_DEBUG
extern int g_debug_show;

#ifdef CONFIG_TOUCHSCREEN_SSL_DEBUG
#define SSL_DEBUG(fmt,arg...) do { if(g_debug_show){ printk("ssd614x: "fmt"\n", ##arg); } } while(0)
#else
#define SSL_DEBUG(fmt,arg...)
#endif
//###############################################################################


//###############################################################################
// Config
//###############################################################################
#define SSL_X_MAX				( 800 )				// resolution X
#define SSL_Y_MAX				( 1280 )				// resolution Y
#define SSL_MAX_X_NODE			( 14 )					// number of NODE X
#define SSL_MAX_Y_NODE			( 22 )					// number of NODE Y

#define SSL_MAX_POINT			( 5 )					// The maximum supported number of fingers.
#define SSL_MAX_NODE			( SSL_MAX_X_NODE * SSL_MAX_Y_NODE )

#define SSL_MAX_RAWDATA_QUEUE	10

#define DELAY_FOR_SIGNAL_DELAY		2	/*us*/
#define DELAY_FOR_TRANSCATION		2
#define DELAY_FOR_POST_TRANSCATION	2

//###############################################################################
//Key Button
//###############################################################################
#ifdef SUPPORT_KEY_BUTTON
#define STR_KEYDATA_BACK_DOWN	0x0001
#define STR_KEYDATA_MENU_DOWN	0x0002
#define STR_KEYDATA_HOME_DOWN	0x0004

#define STR_KEYDATA_BACK_UP		0x0100
#define STR_KEYDATA_MENU_UP		0x0200
#define STR_KEYDATA_HOME_UP		0x0400
#endif	// SUPPORT_KEY_BUTTON

//###############################################################################
// CMD & Reg Addr
//###############################################################################
#define SSL_CHIP_ID				0x0001
#define SSL_ADD_POWERMODE		0x0003
#define SSL_SWRESET_CMD			0x0044

#define SSL_INT_CLEAR_CMD		0x0043
#define SSL_SW_CALIBRATION		0x0040
#define SSL_HW_CALIBRATION		0x0046

#define SSL_TOUCH_MODE			0x0050

#define SSL_INT_FLAG			0x0051
#define SSL_FW_VERSION			0x0052
#define SSL_USR_VERSION			0x0053
#define SSL_TOTAL_Y_NODE		0x0054
#define SSL_TOTAL_X_NODE		0x0055
#define SSL_X_RESOLUTION		0x0056
#define SSL_Y_RESOLUTION		0x0057
#define SSL_ORIENTATION			0x0058
#define SSL_USING_POINT_NUM		0x0059
#define SSL_SENSITIVITY			0x005C
#define SSL_ESD_INT_INTERVAL	0x006A

#define SSL_POINT_SCALE			0x00E0
#define SSL_HW_CAL_INFO			0x00E4

#define SSL_STATUS_LENGTH		0x00F0
#define SSL_POINT_DATA			0x00F1
#define SSL_RAW_DATA			0x00F2
#define SSL_DEBUG_MODE			0x00F3
#define SSL_GRAPH_MODE			0x00F4

#define SSL_GET_KEYDATA			0x00F5
#define SSL_GET_GESTURE			0x00F6
#define SSL_AUX					0x00F8
#define SSL_DOWNLOAD_MODE		0x00F9

#define SSL_FW_VER_WRITE		0x00FA
#define SSL_DATA_VER_WRITE		0x00FB

#define SSL_SAVE_REG			0x00FD

#ifdef SUPPORT_GESTURE

//###############################################################################
// POWER MODE
//###############################################################################
#define POWER_MODE_NM				0x0001
#define POWER_MODE_LPM				0x0002

#define CHECK_GESTURE_KNOCK_ON		0x0400

#define STATUS_CHECK_PALM_GESTURE	0x10
#define STATUS_CHECK_KEY			0x20
#define STATUS_CHECK_AUX			0x40

#define GESTURE_STATUS_KNOCK_CODE	0x08
#define GESTURE_STATUS_KNOCK_ON		0x04
#define GESTURE_STATUS_PALM_REJECT	0x02
#define GESTURE_STATUS_LARGE_PALM	0x01

#define GESTURE_STATUS_KNOCK_ALL	(GESTURE_STATUS_KNOCK_CODE|GESTURE_STATUS_KNOCK_ON)
#endif


//###############################################################################
// IOCTL
//###############################################################################
#define TOUCH_IOCTL_BASE					0xbc
#define TOUCH_IOCTL_GET_FW_VERSION			_IO(TOUCH_IOCTL_BASE, 0)
#define TOUCH_IOCTL_GET_DATA_VERSION		_IO(TOUCH_IOCTL_BASE, 1)
#define TOUCH_IOCTL_GET_X_NODE_NUM			_IO(TOUCH_IOCTL_BASE, 2)
#define TOUCH_IOCTL_GET_Y_NODE_NUM			_IO(TOUCH_IOCTL_BASE, 3)
#define TOUCH_IOCTL_GET_TOTAL_NODE_NUM		_IO(TOUCH_IOCTL_BASE, 4)
#define TOUCH_IOCTL_SET_TOUCH_MODE			_IO(TOUCH_IOCTL_BASE, 5)
#define TOUCH_IOCTL_GET_RAW_DATA			_IO(TOUCH_IOCTL_BASE, 6)
#define TOUCH_IOCTL_GET_X_RESOLUTION		_IO(TOUCH_IOCTL_BASE, 7)
#define TOUCH_IOCTL_GET_Y_RESOLUTION		_IO(TOUCH_IOCTL_BASE, 8)
#define TOUCH_IOCTL_GET_REG					_IO(TOUCH_IOCTL_BASE, 9)
#define TOUCH_IOCTL_SET_REG					_IO(TOUCH_IOCTL_BASE, 10)
#define TOUCH_IOCTL_SET_DOWNLOAD			_IO(TOUCH_IOCTL_BASE, 11)
#define TOUCH_IOCTL_GET_GRAPH_DATA			_IO(TOUCH_IOCTL_BASE, 12)
#define TOUCH_IOCTL_QUEUE_CLEAR				_IO(TOUCH_IOCTL_BASE, 13)
#define TOUCH_IOCTL_GET_GESTURE				_IO(TOUCH_IOCTL_BASE, 14)
#define TOUCH_IOCTL_RESET_TP				_IO(TOUCH_IOCTL_BASE, 15)
#define TOUCH_IOCTL_RESET_TP1				_IO(TOUCH_IOCTL_BASE, 16)
#define TOUCH_IOCTL_DEBUG_TEST				_IO(TOUCH_IOCTL_BASE, 17)
#define TOUCH_IOCTL_RAWDATA					_IO(TOUCH_IOCTL_BASE, 18)


//###############################################################################
// Output function
//###############################################################################
int ssl_get_usr_verion(void);

int ts_read_data(struct i2c_client *client, unsigned short reg, unsigned char *values, unsigned short length);
int ts_write_data(struct i2c_client *client, unsigned short reg, unsigned char *values, unsigned short length);

//###############################################################################
// upgrade function
//###############################################################################
int dl_get_usr_version(void);

int dl_force_upgrade_by_file(char* filename);
int dl_force_upgrade_by_default_file(void);

int dl_force_upgrade_by_data(void);

int dl_auto_update_init(void);
int dl_auto_update_exit(void);

#endif //__SSD614X_H
