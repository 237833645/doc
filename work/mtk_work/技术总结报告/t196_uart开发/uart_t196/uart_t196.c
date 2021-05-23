
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/init.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/reboot.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/vmalloc.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/ctype.h>

#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/workqueue.h>
#include <linux/switch.h>
#include <linux/delay.h>

#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/kthread.h>
#include <linux/time.h>

#include <linux/string.h>

#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/input.h>
#include <linux/of_irq.h>
#include <mt-plat/mt_boot.h>
#include <mt-plat/mt_gpio.h>
#include <mach/gpio_const.h>
#include <mt-plat/mt_gpio_core.h>

#define UART_CTRL_FUNC "UART_CTRL_FUNC"

extern int pinctrl_select_state(struct pinctrl *p, struct pinctrl_state *state);

extern void disable_ear_plug_irq(int disable);
extern void disable_ptt_key_irq(int disable)

//static struct pinctrl *uart_ctrl_pinctrl = NULL;
//static struct pinctrl_state *uart_ctrl_pins_out_high = NULL;
//static struct pinctrl_state *uart_ctrl_pins_out_low = NULL;


//static int uart_ctrl_pinctrl_init(struct platform_device *dev)
//{
//	int ret = 0;
//
//	pr_info(UART_CTRL_FUNC "%s, %d\n", __func__, __LINE__);
//
//    uart_ctrl_pinctrl = devm_pinctrl_get(&dev->dev);
//	if (IS_ERR(uart_ctrl_pinctrl)) {
//		ret = PTR_ERR(uart_ctrl_pinctrl);
//		pr_err(UART_CTRL_FUNC "Cannot find uart_ctrl_pinctrl!\n");
//		return -1;
//	}
//
//	uart_ctrl_pins_out_high = pinctrl_lookup_state(uart_ctrl_pinctrl, "uart_ctrl_pins_out_high");
//	if (IS_ERR(uart_ctrl_pins_out_high)) {
//		ret = PTR_ERR(uart_ctrl_pins_out_high);
//		pr_err(UART_CTRL_FUNC "Cannot find uart_ctrl_pins_out_high pin_cfg!\n");
//		return -1;
//	}
//
//	uart_ctrl_pins_out_low = pinctrl_lookup_state(uart_ctrl_pinctrl, "uart_ctrl_pins_out_low");
//	if (IS_ERR(uart_ctrl_pins_out_low)) {
//		ret = PTR_ERR(uart_ctrl_pins_out_low);
//		pr_err(UART_CTRL_FUNC "Cannot find uart_ctrl_pins_out_low pin_cfg!\n");
//		return -1;
//	}
//
//
//    // test code temp
//    pinctrl_select_state(uart_ctrl_pinctrl, uart_ctrl_pins_out_low);
//
//	pr_info(UART_CTRL_FUNC "%s, %d\n", __func__, __LINE__);
//
//	return ret;
//}




//static int open_uart_volt33(int open)
//{
//    pr_info(UART_CTRL_FUNC "%s, %d\n", __func__, __LINE__);
//    if (open) //open 3.3v en pin
//    {
//        if (NULL != uart_ctrl_pinctrl && NULL != uart_ctrl_pins_out_high)
//            pinctrl_select_state(uart_ctrl_pinctrl, uart_ctrl_pins_out_high);
//    }
//    else     //close 3.3v en pin
//    {
//        if (NULL != uart_ctrl_pinctrl && NULL != uart_ctrl_pins_out_low)
//            pinctrl_select_state(uart_ctrl_pinctrl, uart_ctrl_pins_out_low);
//    }
//}

static int disable_ptt_and_ear_irq(int disable)
{
    pr_info(UART_CTRL_FUNC "%s, %d\n", __func__, __LINE__);
    if(disable) //disable ptt and ear irq handle
    {
        disable_ear_plug_irq(1);
        disable_ptt_key_irq(1);
    }
    else     //enable ptt and ear irq handle
    {
        disable_ear_plug_irq(0);
        disable_ptt_key_irq(0);
    }
}

static int g_uart_ctrl_value = 0;

static ssize_t show_uart_ctrl_access(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", g_uart_ctrl_value);
}

static ssize_t store_uart_ctrl_access(struct device *dev, struct device_attribute *attr,
				    const char *buf, size_t size)
{
	sscanf(buf, "%d", &g_uart_ctrl_value);

    pr_info(UART_CTRL_FUNC "%s, %d g_uart_ctrl_value = %d \n", __func__, __LINE__, g_uart_ctrl_value);

    if(g_uart_ctrl_value == 1)
	{
        disable_ptt_and_ear_irq(1);
        //open_uart_volt33(1);
	}
	else
	{
        disable_ptt_and_ear_irq(0);
        //open_uart_volt33(0);
	}
	return size;
}

static DEVICE_ATTR(uart_ctrl_access, 0664, show_uart_ctrl_access, store_uart_ctrl_access);


static int create_uart_ctrl_file(struct platform_device *dev)
{
	int ret = 0;
	int ret_device_file = 0;

	pr_info(UART_CTRL_FUNC "%s, %d\n", __func__, __LINE__);

    ret_device_file = device_create_file(&(dev->dev), &dev_attr_uart_ctrl_access);
    if (ret_device_file != 0)
    {
        pr_err(UART_CTRL_FUNC "%s, %d\n", __func__, __LINE__);
        return -1;
    }

	pr_info(UART_CTRL_FUNC "%s, %d\n", __func__, __LINE__);

    return ret;
}

static int uart_ctrl_probe(struct platform_device *dev)
{
	int ret = 0;

	pr_info(UART_CTRL_FUNC "%s, %d start\n", __func__, __LINE__);

    //ret = uart_ctrl_pinctrl_init(dev);
    //if (ret != 0)
    //{
    //    pr_err(UART_CTRL_FUNC "%s, %d,  uart_ctrl_pinctrl_init is error!\n", __func__, __LINE__);
    //    return ret;
    //}

    ret = create_uart_ctrl_file(dev);
    if (ret != 0)
    {
        pr_err(UART_CTRL_FUNC "%s, %d,  create_uart_ctrl_file is error!\n", __func__, __LINE__);
        return ret;
    }

	pr_info(UART_CTRL_FUNC "%s, %d end\n", __func__, __LINE__);
	return ret;
}

static int uart_ctrl_remove(struct platform_device *dev)
{
	pr_info(UART_CTRL_FUNC "%s, %d\n", __func__, __LINE__);

	return 0;
}
#ifdef CONFIG_OF
static const struct of_device_id uart_ctrl_of_match[] = {
    { .compatible = "mediatek, uart_ctrl", },
    {},
};
#endif


static struct platform_driver uart_ctrl_driver = {
	.probe = uart_ctrl_probe,
	.remove = uart_ctrl_remove,
	.driver = {
        .owner	= THIS_MODULE,
		.name = "uart_ctrl",
        #ifdef CONFIG_OF
        .of_match_table = uart_ctrl_of_match,
        #endif

	},
};


static int uart_ctrl_mod_init(void)
{
	pr_info(TAG ": %s line=%d\n", __func__, __LINE__);

	if(platform_driver_register(&uart_ctrl_driver) != 0)
	{
		pr_err(TAG "unable to register uart_ctrl driver\n");
		return -1;
	}
	
	return 0;
}

static void uart_ctrl_mod_exit(void)
{
	pr_info(TAG ": %s line=%d\n", __func__, __LINE__);

	platform_driver_unregister(&uart_ctrl_driver);
}

module_init(uart_ctrl_mod_init);
module_exit(uart_ctrl_mod_exit);

MODULE_DESCRIPTION("Rinlink uart_ctrl driver");
MODULE_AUTHOR("AL <wuzhiyong@rinlink.com>");
MODULE_LICENSE("GPL");
