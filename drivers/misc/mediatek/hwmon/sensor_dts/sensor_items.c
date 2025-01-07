/*
* Copyright (C) 2016 ELink Inc.
* ELink_yg@20160105
*/

#include <linux/of.h>
#include <linux/of_irq.h>
#include <cust_alsps.h>
#include <cust_acc.h>
#include <cust_gyro.h>
#include <cust_mag.h>
#include <cust_baro.h>
#include <cust_hmdy.h>
#include <mach/items.h>

#define SENSOR_TAG				  "[Elink Sensor items] "
#define SENSOR_ERR(fmt, args...)	pr_err(SENSOR_TAG fmt, ##args)
#define SENSOR_LOG(fmt, args...)	printk(SENSOR_TAG fmt, ##args)

struct acc_hw *get_accel_items_func(const char *name, struct acc_hw *hw)
{


	SENSOR_LOG("[%s]Kernel items get accel info!\n", __func__);
	if (name == NULL)
		return NULL;

	if(item_exist("sensor.accelerometer.exist")) {
		
		if(item_exist("accelerometer.i2c"))
			hw->i2c_num = item_integer("accelerometer.i2c",0);
		
		if(strstr(name,"bma") != NULL && item_exist("accelerometer.bma.orientation"))
			hw->direction = item_integer("accelerometer.bma.orientation",0);
		
		if(strstr(name,"mc3") != NULL && item_exist("accelerometer.mc3.orientation"))
			hw->direction = item_integer("accelerometer.mc3.orientation",0);

		SENSOR_LOG("[%s] Set acc_hw : hw->i2c_num = %d, hw->direction = %d.\n", __func__, hw->i2c_num, hw->direction);

	} else {
		SENSOR_ERR("Kernel items: sensor accelerometer not exist!.\n");
		return NULL;
	}

	return hw;
}


struct alsps_hw *get_alsps_items_func(const char *name, struct alsps_hw *hw)
{

	struct device_node *node = NULL;
	u32 i2c_num[] = {0};
	int i2c, ret = 0;
	
	SENSOR_LOG("[%s]Kernel items get alsps info!\n", __func__);
	if (name == NULL)
		return NULL;
		
	node = of_find_compatible_node(NULL, NULL, name);
	if (node) {
		if(item_exist("alsps.i2c")){
		
			i2c = item_integer("alsps.i2c",0);
			SENSOR_LOG("[%s] i2c = %d.\n", __func__, i2c);
			
			if(0 == i2c)
				ret = of_property_read_u32_array(node , "i2c_num0", i2c_num, ARRAY_SIZE(i2c_num));
			else if(1 == i2c)
				ret = of_property_read_u32_array(node , "i2c_num1", i2c_num, ARRAY_SIZE(i2c_num));
			else if(2 == i2c)
				ret = of_property_read_u32_array(node , "i2c_num2", i2c_num, ARRAY_SIZE(i2c_num));
			else 
				ret = of_property_read_u32_array(node , "i2c_num3", i2c_num, ARRAY_SIZE(i2c_num));
			
			if (ret == 0)
				hw->i2c_num	=	i2c_num[0];
				
			} else {
				SENSOR_ERR("Kernel items: sensor alsps not exist!.\n");
				return NULL;
			}
	}			

		SENSOR_LOG("[%s] Set alsps_hw : hw->i2c_num = %d.\n", __func__, hw->i2c_num);


	return hw;
}

struct mag_hw *get_mag_items_func(const char *name, struct mag_hw *hw)
{
	int i, ret;
	u32 i2c_num[] = {0};
	u32 i2c_addr[M_CUST_I2C_ADDR_NUM] = {0};
	u32 direction[] = {0};
	u32 power_id[] = {0};
	u32 power_vol[] = {0};
	u32 is_batch_supported[] = {0};
	struct device_node *node = NULL;

	SENSOR_LOG("Device Tree get mag info!\n");
	if (name == NULL)
		return NULL;

	node = of_find_compatible_node(NULL, NULL, name);
	if (node) {
		ret = of_property_read_u32_array(node , "i2c_num", i2c_num, ARRAY_SIZE(i2c_num));
		if (ret == 0)
			hw->i2c_num	=	i2c_num[0];

		ret = of_property_read_u32_array(node , "i2c_addr", i2c_addr, ARRAY_SIZE(i2c_addr));
		if (ret == 0) {
			for (i = 0; i < M_CUST_I2C_ADDR_NUM; i++)
				hw->i2c_addr[i]   = i2c_addr[i];
		}

		ret = of_property_read_u32_array(node , "direction", direction, ARRAY_SIZE(direction));
		if (ret == 0)
			hw->direction = direction[0];

		ret = of_property_read_u32_array(node , "power_id", power_id, ARRAY_SIZE(power_id));
		if (ret == 0) {
			if (power_id[0] == 0xffff)
				hw->power_id = -1;
			else
				hw->power_id	=	 power_id[0];
		}

		ret = of_property_read_u32_array(node , "power_vol", power_vol, ARRAY_SIZE(power_vol));
		if (ret == 0)
			hw->power_vol	 =	  power_vol[0];

		ret = of_property_read_u32_array(node , "is_batch_supported", is_batch_supported,
			ARRAY_SIZE(is_batch_supported));
		if (ret == 0)
			hw->is_batch_supported		   = is_batch_supported[0];
	} else {
		SENSOR_ERR("Device Tree: can not find mag node!. Go to use old cust info\n");
		return NULL;
	}
	return hw;
}

struct gyro_hw *get_gyro_items_func(const char *name, struct gyro_hw *hw)
{
	int i, ret;
	u32 i2c_num[] = {0};
	u32 i2c_addr[C_CUST_I2C_ADDR_NUM] = {0};
	u32 direction[] = {0};
	u32 power_id[] = {0};
	u32 power_vol[] = {0};
	u32 firlen[] = {0};
	u32 is_batch_supported[] = {0};
	struct device_node *node = NULL;

	SENSOR_LOG("Device Tree get gyro info!\n");
	if (name == NULL)
		return NULL;

	node = of_find_compatible_node(NULL, NULL, name);
	if (node) {
		ret = of_property_read_u32_array(node , "i2c_num", i2c_num, ARRAY_SIZE(i2c_num));
		if (ret == 0)
			hw->i2c_num	=	i2c_num[0];

		ret = of_property_read_u32_array(node , "i2c_addr", i2c_addr, ARRAY_SIZE(i2c_addr));
		if (ret == 0) {
			for (i = 0; i < GYRO_CUST_I2C_ADDR_NUM; i++)
				hw->i2c_addr[i] = i2c_addr[i];
		}

		ret = of_property_read_u32_array(node , "direction", direction, ARRAY_SIZE(direction));
		if (ret == 0)
			hw->direction = direction[0];

		ret = of_property_read_u32_array(node , "power_id", power_id, ARRAY_SIZE(power_id));
		if (ret == 0) {
			if (power_id[0] == 0xffff)
				hw->power_id = -1;
			else
				hw->power_id	=	power_id[0];
		}

		ret = of_property_read_u32_array(node , "power_vol", power_vol, ARRAY_SIZE(power_vol));
		if (ret == 0)
			hw->power_vol	=	power_vol[0];

		ret = of_property_read_u32_array(node , "firlen", firlen, ARRAY_SIZE(firlen));
		if (ret == 0)
			hw->firlen	=	firlen[0];

		ret = of_property_read_u32_array(node , "is_batch_supported", is_batch_supported,
			ARRAY_SIZE(is_batch_supported));
		if (ret == 0)
			hw->is_batch_supported		 = is_batch_supported[0];
	} else {
		SENSOR_ERR("Device Tree: can not find gyro node!. Go to use old cust info\n");
		return NULL;
	}
	return hw;
}

struct baro_hw *get_baro_items_func(const char *name, struct baro_hw *hw)
{
	int i, ret;
	u32 i2c_num[] = {0};
	u32 i2c_addr[C_CUST_I2C_ADDR_NUM] = {0};
	u32 direction[] = {0};
	u32 power_id[] = {0};
	u32 power_vol[] = {0};
	u32 firlen[] = {0};
	u32 is_batch_supported[] = {0};
	struct device_node *node = NULL;

	SENSOR_LOG("Device Tree get gyro info!\n");
	if (name == NULL)
		return NULL;

	node = of_find_compatible_node(NULL, NULL, name);
	if (node) {
		ret = of_property_read_u32_array(node , "i2c_num", i2c_num, ARRAY_SIZE(i2c_num));
		if (ret == 0)
			hw->i2c_num	=	i2c_num[0];

		ret = of_property_read_u32_array(node , "i2c_addr", i2c_addr, ARRAY_SIZE(i2c_addr));
		if (ret == 0) {
			for (i = 0; i < GYRO_CUST_I2C_ADDR_NUM; i++)
				hw->i2c_addr[i] = i2c_addr[i];
		}

		ret = of_property_read_u32_array(node , "direction", direction, ARRAY_SIZE(direction));
		if (ret == 0)
			hw->direction = direction[0];

		ret = of_property_read_u32_array(node , "power_id", power_id, ARRAY_SIZE(power_id));
		if (ret == 0) {
			if (power_id[0] == 0xffff)
				hw->power_id = -1;
			else
				hw->power_id	=	power_id[0];
		}

		ret = of_property_read_u32_array(node , "power_vol", power_vol, ARRAY_SIZE(power_vol));
		if (ret == 0)
			hw->power_vol	=	power_vol[0];

		ret = of_property_read_u32_array(node , "firlen", firlen, ARRAY_SIZE(firlen));
		if (ret == 0)
			hw->firlen	=	firlen[0];

		ret = of_property_read_u32_array(node , "is_batch_supported", is_batch_supported,
			ARRAY_SIZE(is_batch_supported));
		if (ret == 0)
			hw->is_batch_supported		 = is_batch_supported[0];
	} else {
		SENSOR_ERR("Device Tree: can not find gyro node!. Go to use old cust info\n");
		return NULL;
	}
	return hw;
}

struct hmdy_hw *get_hmdy_items_func(const char *name, struct hmdy_hw *hw)
{
	int i, ret;
	u32 i2c_num[] = {0};
	u32 i2c_addr[C_CUST_I2C_ADDR_NUM] = {0};
	u32 direction[] = {0};
	u32 power_id[] = {0};
	u32 power_vol[] = {0};
	u32 firlen[] = {0};
	u32 is_batch_supported[] = {0};
	struct device_node *node = NULL;

	SENSOR_LOG("Device Tree get gyro info!\n");
	if (name == NULL)
		return NULL;

	node = of_find_compatible_node(NULL, NULL, name);
	if (node) {
		ret = of_property_read_u32_array(node , "i2c_num", i2c_num, ARRAY_SIZE(i2c_num));
		if (ret == 0)
			hw->i2c_num	=	i2c_num[0];

		ret = of_property_read_u32_array(node , "i2c_addr", i2c_addr, ARRAY_SIZE(i2c_addr));
		if (ret == 0) {
			for (i = 0; i < GYRO_CUST_I2C_ADDR_NUM; i++)
				hw->i2c_addr[i] = i2c_addr[i];
		}

		ret = of_property_read_u32_array(node , "direction", direction, ARRAY_SIZE(direction));
		if (ret == 0)
			hw->direction = direction[0];

		ret = of_property_read_u32_array(node , "power_id", power_id, ARRAY_SIZE(power_id));
		if (ret == 0) {
			if (power_id[0] == 0xffff)
				hw->power_id = -1;
			else
				hw->power_id	=	power_id[0];
		}

		ret = of_property_read_u32_array(node , "power_vol", power_vol, ARRAY_SIZE(power_vol));
		if (ret == 0)
			hw->power_vol	=	power_vol[0];

		ret = of_property_read_u32_array(node , "firlen", firlen, ARRAY_SIZE(firlen));
		if (ret == 0)
			hw->firlen	=	firlen[0];

		ret = of_property_read_u32_array(node , "is_batch_supported", is_batch_supported,
			ARRAY_SIZE(is_batch_supported));
		if (ret == 0)
			hw->is_batch_supported		 = is_batch_supported[0];
	} else {
		SENSOR_ERR("Device Tree: can not find gyro node!. Go to use old cust info\n");
		return NULL;
	}
	return hw;
}

