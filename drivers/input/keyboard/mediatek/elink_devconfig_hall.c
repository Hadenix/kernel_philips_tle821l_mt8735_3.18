#include "kpd.h"
#include <linux/wakelock.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/clk.h>
#include <linux/irqchip/mt-eic.h>
#include "elink_devconfig_hall.h"
#include <mach/items.h>

static bool kpd_pwrkey_state = false;
#define DTS_HALL_NAME "mediatek, mhall-eint"
static unsigned int mhall_irq;
#define KPD_PWRKEY_MAP KEY_POWER
extern bool lcd_hall_status; //on 1 ;off 0
static bool is_irq_trigger_low = false;
#define MASK_IRQ(a) (u32)(~(0x1<<8) & (a))
#define KGPIO(a) (u32)((0x8<<31) | (a))

static void kpd_hall_eint_handler_event(unsigned long data);
static DECLARE_TASKLET(kpde_hall_tasklet, kpd_hall_eint_handler_event, 0);

void kpd_hall_eint_handler_event(unsigned long data)
{

	kpd_pwrkey_state = !kpd_pwrkey_state;
	
	printk("hall [%s] kpd_pwrkey_state irq status %d\n",__func__, !kpd_pwrkey_state);

	if(kpd_pwrkey_state)
	{  
		//open
	   if(!lcd_hall_status)
	   	{
			input_report_key(kpd_input_dev, KPD_PWRKEY_MAP, 1);
			input_sync(kpd_input_dev);
			input_report_key(kpd_input_dev, KPD_PWRKEY_MAP, 0);
			input_sync(kpd_input_dev);
			printk("hall [%s] --- Elink hall driver Tigger LCM OFF --- \n",__func__);
	   	}
		if(is_irq_trigger_low)			
			irq_set_irq_type(mhall_irq, IRQF_TRIGGER_LOW);
		else
			irq_set_irq_type(mhall_irq, IRQF_TRIGGER_HIGH);
	}
	else
	{		
		//close
		 if(lcd_hall_status)
	     {
			input_report_key(kpd_input_dev, KPD_PWRKEY_MAP, 1);
			input_sync(kpd_input_dev);
			input_report_key(kpd_input_dev, KPD_PWRKEY_MAP, 0);
			input_sync(kpd_input_dev);
			printk("hall [%s] --- Elink hall driver Tigger LCM ON --- \n",__func__);
	     }
		 if(is_irq_trigger_low)			
			irq_set_irq_type(mhall_irq, IRQF_TRIGGER_HIGH);
		else
			irq_set_irq_type(mhall_irq, IRQF_TRIGGER_LOW);
	}
	
	kpd_print("hall report Linux keycode = %u\n", KPD_PWRKEY_MAP);
	
	//input_sync(kpd_input_dev);	
	enable_irq(mhall_irq);

}

static irqreturn_t kpd_hall_eint_handler(int irq, void *desc)
{
	printk("hall ---- HALL EINT Tigger ---- \n");
	disable_irq_nosync(mhall_irq);
	tasklet_schedule(&kpde_hall_tasklet);
	return IRQ_HANDLED;
}

int mt_hall_eint_register(struct platform_device *pdev)
{
	int r;
	struct device_node *node = NULL;
	u32 d_ints[2] = { 0, 0 };
	u32 i_ints[2] = { 0, 0 };
	struct pinctrl *pinctrl;
	struct pinctrl_state *pins_default;
	struct pinctrl_state *pins_cfg;
	
	pinctrl = devm_pinctrl_get(&pdev->dev);
	if (IS_ERR(pinctrl)) {
		r = PTR_ERR(pinctrl);
		printk("Cannot find hall pinctrl!\n");
	}
	pins_default = pinctrl_lookup_state(pinctrl, "pin_default");
	if (IS_ERR(pins_default)) {
		r = PTR_ERR(pins_default);
		printk("Cannot find hall pinctrl default!\n");
	}

	pins_cfg = pinctrl_lookup_state(pinctrl, "pin_cfg");
	if (IS_ERR(pins_cfg)) {
		r = PTR_ERR(pins_cfg);
		printk("Cannot find hall pinctrl pin_cfg!\n");
	}

#if 1
	if(!item_exist("hall_eint.used") || !item_integer("hall_eint.used",0)) {
		printk("hall [%s] <hall_eint.used> not open in items.\n", __func__);
		return 0;
	}
#endif

	node = of_find_compatible_node(NULL, NULL, DTS_HALL_NAME);
	of_property_read_u32_array(node , "debounce", d_ints, ARRAY_SIZE(d_ints));
	of_property_read_u32_array(node , "interrupts", i_ints, ARRAY_SIZE(i_ints));

	mhall_irq = irq_of_parse_and_map(node, 0);
	
	printk("hall [%s] debounce = %d, interrupts = %d, mhall_irq = %d\n", __func__, d_ints[1], i_ints[1], mhall_irq);
	
	if((i_ints[1] & IRQF_TRIGGER_LOW)) { //  || (i_ints[1] & IRQF_TRIGGER_FALLING)
		is_irq_trigger_low = true;
	}
	if(is_irq_trigger_low) {
		kpd_pwrkey_state = false;
	} else {
		kpd_pwrkey_state = true;
	}

	gpio_request(KGPIO(d_ints[0]), "hall-sensor");
	gpio_set_debounce(KGPIO(d_ints[0]), d_ints[1]+10);
	pinctrl_select_state(pinctrl, pins_cfg); 
	r = request_irq(mhall_irq, kpd_hall_eint_handler, IRQF_TRIGGER_LOW, "MHALL-eint", NULL);
	if (r) {
		printk("hall [%s]register IRQ<%d> failed (%d)\n", __func__, mhall_irq, r);
		return r;
	}

	enable_irq(mhall_irq);

	return 0;
}



