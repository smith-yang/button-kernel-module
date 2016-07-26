#include <linux/module.h>    // included for all kernel modules
#include <linux/moduleparam.h>
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/gpio.h>

//#define EXTERNAL_PARAM    1
#define COUNT_IRQ   1

#ifdef EXTERNAL_PARAM
static int gpio_num = 0;//GP2_4 = GPIO932
#else
static int gpio_num = 932;//GP2_4 = GPIO932
#endif
static int irq_num = 0;

#ifdef COUNT_IRQ
static int irq_count = 0;
#endif

#ifdef EXTERNAL_PARAM
module_param(gpio_num, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(gpio_num, "GPIO number for IRQ");
#endif

static irqreturn_t btn_irq(int irq, void *data_)
{
    printk("%s:got irq:%d\n", __func__, irq);
#ifdef COUNT_IRQ
    printk("%s:irq_count:%d\n", __func__, irq_count++);
#endif
	return IRQ_HANDLED;
}

static int __init button_init(void)
{
	int ret;

    printk(KERN_INFO "button module init.\n");
    printk(KERN_INFO "%s:gpio_num:%d.\n", __func__, gpio_num);

    if(!gpio_num){
		printk("%s: wrong gpio_num: %d\n", __func__, gpio_num);
		goto error;
    }
	ret = gpio_request(gpio_num, "BUTTON IRQ");
	if (ret < 0) {
		printk("%s: failed to request GPIO: %d\n", __func__, gpio_num);
		ret = -ENODEV;
	}
	ret = gpio_direction_input(gpio_num);
	if (ret < 0) {
		printk("%s: failed to set GPIO %d as input.\n", __func__, gpio_num);
		ret = -EINVAL;
	}
	irq_num = gpio_to_irq(gpio_num);
	ret = request_irq(irq_num, btn_irq,
	                  IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "button irq", NULL);

	if (ret) {
		printk("Could not get button IRQ\n");
	}
error:
    return 0;    // Non-zero return means that the module couldn't be loaded.
}

static void __exit button_exit(void)
{
    printk(KERN_INFO "capsensor module exit.\n");
    if(irq_num){
        printk("%s:free irq %d\n", __func__, irq_num);
        free_irq(irq_num, NULL);
    }
    if(gpio_num){
        printk("%s:free gpio %d\n", __func__, gpio_num);
        gpio_free(gpio_num);
    }
}

module_init(button_init);
module_exit(button_exit);

MODULE_VERSION("0.1");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Smith Yang<smith_yang@pegatroncorp.com>");
MODULE_DESCRIPTION("A Simple button irq module");
