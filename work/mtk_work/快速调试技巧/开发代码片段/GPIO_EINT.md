# gpio控制代码

## GPIO设备驱动控制代码片段

- gpio lk阶段使用代码片段

```c
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
//    mt_set_gpio_pull_enable(GPIO_ISL_97900_CE, 1); //是否允许上下拉
    mt_set_gpio_dir(GPIO_ISL_97900_CE, 1);
    mt_set_gpio_out(GPIO_ISL_97900_CE, 1);
    MDELAY(200);
    GPIO_CTRL_FUNC("ISL_97900_RST end\n");
}

读取GPIO输出电平状态
S32 GET_ISL_97900_RST_OUT_PIN_LEVEL()
{
    S32 level;
    mt_set_gpio_mode(GPIO_ISL_97900_CE, 0);
    mt_set_gpio_dir(GPIO_ISL_97900_CE, 0);    //设置GPIO  IN状态
//    mt_set_gpio_pull_enable(GPIO_ISL_97900_CE, 1); //是否允许上下拉
    level = mt_get_gpio_out(GPIO_ISL_97900_CE);
    return level;
}


```

- gpio kernel阶段使用代码片段

```c


```

## GPIO设备驱动控制代码移植包

### GPIO设备驱动移植代码(只控制gpio输出)

[GPIO设备控制驱动移植代码 gpio_ctrl.zip](./res/gpio_ctrl.zip)  
[GPIO带lk操作的驱动移植代码gpio_ctrl_lk.zip](./res/gpio_ctrl_lk.zip)

### GPIO设备驱动移植代码(中断及GPIO读取电平及按键input设备驱动)


## 权限控制（略）