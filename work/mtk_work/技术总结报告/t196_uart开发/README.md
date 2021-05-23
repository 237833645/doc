# 串口通信方式替代usb方式写号

## 背景

T196项目没有预留usb接口，终端客户要想给机器烧入信息需要通过外接一个usb的转接板插入到主板对应插脚上，才可以使用usb相关功能，客户觉得非常的麻烦。
t196主板后来改变增加了uart的接口到音频接头上，希望通过uart的方式来实现客户烧入相关数据。

## 问题点

- 主板的特殊走线方式，在使用uart总线的时候会影响到ptt和耳机插入识别口，会导致uart数据通信失败
- 客户烧入主板的数据是通过电脑端的工具去烧入的，以前用的是usb协议，现在要换成serial协议，所以需要先开启serial功能.

## 思路

1. 解决ptt及耳机插入中断

   - 在uart通信功能使用前屏蔽ptt及耳机插入中断功能

2. 在电脑工具连接设备前要开启serial功能，需要有触发状态，使用结束后需要关闭serial功能，并解除ptt和耳机中断屏蔽

   - 设备没有屏幕，通过2个组合按键同时按下来开启serial功能，电脑程序在使用写号功能后控制设备重启，或者手动重启设备。

## 过程

~~- 在2个按键中断处理函数内部添加状态值，驱动底层直接判断2个按键是否同时按下~~
    ~~- 驱动程序需要开启一个线程定时2s去检查2个按键是否同时按下，如果检查到了
    - 屏蔽ptt和耳机插入中断功能
    - 开启uart电平转换使能功能
    - 将开启uart功能的状态告知android系统~~

- android系统去获取2个按键同时按下的状态,直接控制设备驱动节点,用于打开uart的电平转换芯片和屏蔽ptt及耳机中断

- 驱动动开发过程
  - dws配置
  - dts配置
  - 屏蔽中断功能编写
  - 设备控制节点编写

## 临时数据

### disable PTT和耳机中断

    - PTT中断 drivers/input/keyboard/mediatek/kpd.c

    ```code

    /* wangfeng add for t196 */
    void disable_ptt_key_irq(int disable)
    {
        pr_info("wangfeng disable_ptt_key_irq status = %d\n", disable);
        if(disable == 1)
            disable_irq_nosync(kp_irqnr);
        else
            enable_irq(kp_irqnr);
    }
    EXPORT_SYMBOL(disable_ptt_key_irq);
    ```

    - EAR中断（耳机插入中断） 文件 "drivers/misc/mediatek/accdet/mt6570/accdet.c"

    ```code
     /* wangfeng add for t196 */
     void disable_ear_plug_irq(int disable)
     {
        pr_info("wangfeng disable_ear_plug_irq status = %d\n", disable);
        if(disable == 1)
            disable_irq_nosync(accdet_irq);
        else
            enable_irq(accdet_irq);
     }
     EXPORT_SYMBOL(disable_ear_plug_irq);
    ```

### dws、dts编写

    - arch/arm/boot/dts/t196.dts

    ```c
     AUD_pins_extamp_high: extamphigh {
        pins_cmd_dat {
         - pins = <PINMUX_GPIO3__FUNC_GPIO3>;
         + pins = <PINMUX_GPIO2__FUNC_GPIO2>;
          slew-rate = <1>;
          output-high;
        };
     };
     AUD_pins_extamp_low: extamplow {
        pins_cmd_dat {
         - pins = <PINMUX_GPIO3__FUNC_GPIO3>;
         + pins = <PINMUX_GPIO2__FUNC_GPIO2>;
          slew-rate = <1>;
          output-low;
        };
    ```

    ```code
    +/* wangfeng add */
    +&pio {
    +       uart_ctrl_pins_default: default {
    +       };
    +       uart_ctrl_pins_out_high: uart_ctrl_pins_out_high{
    +        pins_cmd_dat {
    +                       pins = <PINMUX_GPIO3__FUNC_GPIO3>;
    +                       slew-rate = <1>;
    +                       output-high;
    +        };
    +    };
    +    uart_ctrl_pins_out_low: uart_ctrl_pins_out_low{
    +        pins_cmd_dat {
    +                       pins = <PINMUX_GPIO3__FUNC_GPIO3>;
    +                       slew-rate = <1>;
    +                       output-low;
    +        };
    +    };
    +};
    +&uart_ctrl {
    +       pinctrl-names = "default","uart_ctrl_pins_out_high","uart_ctrl_pins_out_low";
    +       pinctrl-0 = <&uart_ctrl_pins_default>;
    +       pinctrl-1 = <&uart_ctrl_pins_out_high>;
    +       pinctrl-2 = <&uart_ctrl_pins_out_low>;
    +};
    +/* wangfeng add */

    ```

    - arch/arm/boot/dts/mt6570.dtsi

    ```code
    +       /*wangfeng add*/
    +       uart_ctrl:uart_ctrl {
    +               compatible = "mediatek, uart_ctrl";
    +       };
    +       /*wangfeng add*/
    ```

### 设备驱动程序编写

### 调试

- [Windows系统串口调试工具O-ComTool](./O-ComTool_V1.1.1.exe)

- [Android端串口调试工具github源码](./SerialPortHelper.zip)

  - <https://github.com/xzlAwin/SerialPortHelper>

- [Android端串口调试工具APK](./SerialPortHelper.apk)

## 总结

在PCB2.0及之前，UART电平转换的使能脚用到了耳机pa上，所以改动量巨大，所以就不再对这个3.3的电平使能脚进行控制，之前系统默认这个脚就是高电平，保证最小改动
