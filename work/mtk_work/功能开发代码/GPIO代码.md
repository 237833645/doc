# GPIO

- [GPIO](#gpio)
  - [GPIO设备驱动控制代码片段及介绍](#gpio设备驱动控制代码片段及介绍)
    - [lk阶段](#lk阶段)
    - [kernel阶段](#kernel阶段)
  - [GPIO设备驱动控制代码移植包(只控制gpio输出)](#gpio设备驱动控制代码移植包只控制gpio输出)
  - [权限控制（略)](#权限控制略)

## GPIO设备驱动控制代码片段及介绍

### lk阶段

- lk阶段gpio操作对应表

| lk阶段gpio Api  | 描述  |
|---|---|
| s32 mt_set_gpio_dir(u32 pin, u32 dir) |  direction |
| s32 mt_get_gpio_dir(u32 pin)  | direction |
| s32 mt_set_gpio_pull_enable(u32 pin, u32 enable) | pull enable |
| s32 s32 mt_get_gpio_pull_enable(u32 pin) | pull enable |
| s32 mt_set_gpio_pull_select(u32 pin, u32 select) | pull select |
| s32 mt_get_gpio_pull_select(u32 pin) | pull select |
| s32 mt_set_gpio_smt(u32 pin, u32 enable) | schmitt trigger |
| s32 mt_get_gpio_smt(u32 pin) | schmitt trigger |
| s32 mt_set_gpio_ies(u32 pin, u32 enable) | IES |
| s32 mt_get_gpio_ies(u32 pin) | IES |
| s32 mt_set_gpio_out(u32 pin, u32 output) | input/output |
|　s32 mt_get_gpio_out(u32 pin) | input/output |
| s32 mt_get_gpio_in(u32 pin) | input/output |
| s32 mt_set_gpio_mode(u32 pin, u32 mode) | mode control |
| s32 mt_get_gpio_mode(u32 pin) | mode control |

- lk阶段gpio操作代码片段

```code
#include <platform/mt_gpio.h>
#include <platform/mt_pmic.h>
#include <cust_gpio_usage.h>

#define GPIO_CTRL_DEBUG_CODE
#define GPIO_CTRL_DEVICE "GPIO_CTRL"

#ifdef GPIO_CTRL_DEBUG_CODE
    #undef GPIO_CTRL_DEBUG
    #define GPIO_CTRL_DEBUG(a,arg...)   printf(GPIO_CTRL_DEVICE ": " a, ##arg)
    #define GPIO_CTRL_FUNC()            printf(GPIO_CTRL_DEVICE ": %s line=%d\n", __func__, __LINE__)
#else
    #define GPIO_CTRL_DEBUG(arg...)
    #define GPIO_CTRL_FUNC()
#endif

#define MDELAY(n)   mdelay(n)

#define GPIO_ISL_97900_CE              (GPIO76 | 0x80000000)  //EB_BL

extern void ISL_97900_RST(void);

//GPIO输出
void ISL_97900_RST(void)
{
    GPIO_CTRL_FUNC("ISL_97900_RST start\n");

    mt_set_gpio_mode(GPIO_ISL_97900_CE, 0);
    mt_set_gpio_dir(GPIO_ISL_97900_CE, 1);
    mt_set_gpio_out(GPIO_ISL_97900_CE, 1);
    MDELAY(200);
    GPIO_CTRL_FUNC("ISL_97900_RST end\n");
}
```

### kernel阶段

- dws配置(略)

- dts配置

```code
gpio_ctrl: gpio_ctrl@0{
  compatible = "mediatek,gpio_ctrl";
};

&pio {
    gpio_ctrl_default: gpio_ctrl_default@0 {

    };

    gpio_ctrl_1_high: gpio_ctrl_1_high@0{
        pins_cmd0_dat {
            pins = <PINMUX_GPIO3__FUNC_GPIO3>;
            slew-rate = <1>;
            output-high;
        };
    };
    gpio_ctrl_1_low: gpio_ctrl_1_low@0{
        pins_cmd0_dat {
            pins = <PINMUX_GPIO3__FUNC_GPIO3>;
            slew-rate = <1>;
            output-low;
        };
    };
};

&gpio_ctrl {
    pinctrl-names = "default", "gpio_ctrl_1_high", "gpio_ctrl_1_low";
    pinctrl-0 = <&gpio_ctrl_default>;
    pinctrl-1 = <&gpio_ctrl_1_high>;
    pinctrl-2 = <&gpio_ctrl_1_low>;
    status = "okay";
};

```

- kernel阶段gpio操作代码片段

```code
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/delay.h>

#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/input.h>

#define GPIO_CTRL_DEBUG_CODE
#define GPIO_CTRL_DEVICE "GPIO_CTRL"

#ifdef GPIO_CTRL_DEBUG_CODE
#undef GPIO_CTRL_DEBUG
#define GPIO_CTRL_DEBUG(a,arg...) pr_err(GPIO_CTRL_DEVICE ": " a, ##arg)
#define GPIO_CTRL_FUNC()          pr_err(GPIO_CTRL_DEVICE ": %s line=%d\n", __func__, __LINE__)
#else
#define GPIO_CTRL_DEBUG(arg...)
#define GPIO_CTRL_FUNC()
#endif


static struct pinctrl *gpio_ctrl_pin_ctrl= NULL;
static struct pinctrl_state *gpio_ctrl_1_high  = NULL;
static struct pinctrl_state *gpio_ctrl_1_low = NULL;

static dev_t gpio_ctrl_devno;
static struct class *gpio_ctrl_class = NULL;
static struct device *gpio_ctrl_devices = NULL;
#define GPIO_CTRL_DEVNAME            "gpio_ctrl"


static int gpio_ctrl_pinctrl_init(struct platform_device *pdev) {
  int ret = 0;
  GPIO_CTRL_FUNC();
  gpio_ctrl_pin_ctrl = devm_pinctrl_get(&pdev->dev);
  if (IS_ERR(gpio_ctrl_pin_ctrl)) {
    dev_err(&pdev->dev, "Cannot find gpio_ctrl_pin_ctrl!");
    ret = PTR_ERR(gpio_ctrl_pin_ctrl);
    printk("%s devm_pinctrl_get fail!\n", __func__);
  }

  /*ctrl 1 */
  gpio_ctrl_1_high =
      pinctrl_lookup_state(gpio_ctrl_pin_ctrl, "gpio_ctrl_1_high");
  if (IS_ERR(gpio_ctrl_1_high)) {
    ret = PTR_ERR(gpio_ctrl_1_high);
    printk("%s : pinctrl err, gpio_ctrl_1_high\n", __func__);
  }

  gpio_ctrl_1_low = pinctrl_lookup_state(gpio_ctrl_pin_ctrl, "gpio_ctrl_1_low");
  if (IS_ERR(gpio_ctrl_1_low)) {
    ret = PTR_ERR(gpio_ctrl_1_low);
    printk("%s : pinctrl err, gpio_ctrl_1_low\n", __func__);
  }

  return ret;
}

static ssize_t gpio_ctrl_set_en(struct device *dev,
                                struct device_attribute *attr, const char *buf,
                                size_t count) {
  int status = 0;
  int pinctrl_num = 0;
  GPIO_CTRL_FUNC();
  sscanf(buf, "%d %d", &pinctrl_num, &status);

  if (gpio_ctrl_pin_ctrl == NULL) {
    printk(
        "\n\n\n\n\ gpio_ctrl_pin_ctrl == 0 Ponit Error !!!!!!!!!!!!! \n\n\n\n");
    return -1;
  }

  if (status) {
    switch (pinctrl_num) {
    case 1:
      if (gpio_ctrl_1_high != 0)
        pinctrl_select_state(gpio_ctrl_pin_ctrl, gpio_ctrl_1_high);
      break;
    default:
      break;
    }

  } else {
    switch (pinctrl_num) {
    case 1:
      if (gpio_ctrl_1_low != 0)
        pinctrl_select_state(gpio_ctrl_pin_ctrl, gpio_ctrl_1_low);
      break;
    default:
      break;
    }
  }
  return count;
}

static ssize_t gpio_ctrl_get_en(struct device *dev,
                                struct device_attribute *attr, char *buf) {
  ssize_t len = 0;

  return len;
}

static DEVICE_ATTR(gpio_ctrl_en, 0660, gpio_ctrl_get_en, gpio_ctrl_set_en);

static struct device_attribute *gpio_ctrl_attr_list[] = {
    &dev_attr_gpio_ctrl_en,
};

static int gpio_ctrl_create_attr(struct device *dev) {
  int idx, err = 0;
  GPIO_CTRL_FUNC();
  int num = (int)sizeof(gpio_ctrl_attr_list) / sizeof(gpio_ctrl_attr_list[0]);

  if (!dev)
    return -EINVAL;

  for (idx = 0; idx < num; idx++) {
    device_create_file(dev, gpio_ctrl_attr_list[idx]);
  }

  return err;
}

static int gpio_ctrl_probe(struct platform_device *pdev) {
  int ret = 0;

  GPIO_CTRL_FUNC();

  ret = gpio_ctrl_pinctrl_init(pdev);
  if (ret != 0) {
    printk("[%s] failed to init gpio_ctrl pinctrl.\n", __func__);
    return ret;
  } else {
    printk("[%s] Success to init gpio_ctrl pinctrl.\n", __func__);
  }

  ret = alloc_chrdev_region(&gpio_ctrl_devno, 0, 1, GPIO_CTRL_DEVNAME);
  if (ret) {
    printk("[gpio_ctrl] alloc_chrdev_region fail: %d\n", ret);
    goto exit_check_functionality_failed;
  } else {
    printk("[gpio_ctrl] major: %d, minor: %d\n", MAJOR(gpio_ctrl_devno),
           MINOR(gpio_ctrl_devno));
  }

  gpio_ctrl_class = class_create(THIS_MODULE, "gpio_ctrl");
  if (IS_ERR(gpio_ctrl_class)) {
    printk("[gpio_ctrl_probe] Unable to create class, err = %d\n",
           (int)PTR_ERR(gpio_ctrl_class));
    goto exit_check_functionality_failed;
  }

  gpio_ctrl_devices = (struct device *)device_create(
      gpio_ctrl_class, NULL, gpio_ctrl_devno, NULL, GPIO_CTRL_DEVNAME);
  if (NULL == gpio_ctrl_devices) {
    printk("[gpio_ctrl_probe] device_create fail\n");
    goto exit_check_functionality_failed;
  }

  if (gpio_ctrl_create_attr(gpio_ctrl_devices))
    printk("[gpio_ctrl_probe] create_attr fail\n");

  return 0;

exit_check_functionality_failed:
  return ret;
}

static int gpio_ctrl_remove(struct platform_device *pdev) {
  GPIO_CTRL_FUNC();
  return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id gpio_ctrl_of_match[] = {
    {
        .compatible = "mediatek,gpio_ctrl",
    },
    {},
};
#endif

const struct dev_pm_ops gpio_ctrl_pm_os = {
    .suspend = NULL,
    .resume = NULL,
};

static struct platform_driver gpio_ctrl_driver = {
    .probe = gpio_ctrl_probe,
    .shutdown = NULL,
    .remove = gpio_ctrl_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "gpio_ctrl",
        .pm = &gpio_ctrl_pm_os,
#ifdef CONFIG_OF
        .of_match_table = gpio_ctrl_of_match,
#endif
    }};

static int gpio_ctrl_mod_init(void) {
  GPIO_CTRL_FUNC();
  if (platform_driver_register(&gpio_ctrl_driver) != 0) {
    GPIO_CTRL_DEBUG("unable to register gpio_ctrl driver\n");
    return -1;
  }
  return 0;
}

static void gpio_ctrl_mode_exit(void) {
  GPIO_CTRL_FUNC();
  platform_driver_unregister(&gpio_ctrl_driver);
}

module_init(gpio_ctrl_mod_init);
module_exit(gpio_ctrl_mode_exit);

MODULE_DESCRIPTION("Rinlink gpio_gpio_ctrl driver");
MODULE_AUTHOR("maoweihua@rinlink.vip");
MODULE_LICENSE("GPL");

```

## GPIO设备驱动控制代码移植包(只控制gpio输出)

[GPIO设备控制驱动移植代码 gpio_ctrl.zip](./res/gpio_ctrl.zip)  
[GPIO带lk操作的驱动移植代码gpio_ctrl_lk.zip](./res/gpio_ctrl_lk.zip)

## 权限控制（略)
