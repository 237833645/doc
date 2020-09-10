# mtk android uart使用

参考文档 [MTK Android Driver：UART](http://www.mamicode.com/info-detail-415990.html)

## uart口配置

  ```code
  #define CFG_UART_LOG    (UART4) //(UART1)
  #define CFG_UART_META  (UART4) //(UART1)

  或者

  CFG_UART_LOG :=UART4  #UART1
  CFG_UART_META :=UART4 #UART1

  ```

## UART port的permission

  ```code
  chmod 0666 /dev/ttyMT0
  chown system system /dev/ttyMT0
  ```

## uart物理端口与系统设备名称对应关系

| 物理端口 | 设备名称    |
| ---   | ---         |
| uart1 | /dev/ttyMT0 |
| uart2 | /dev/ttyMT1 |
| uart3 | /dev/ttyMT2 |
| uart4 | /dev/ttyMT3 |

## 平台相关serialport api代码

- 上层apk
  
  ```code
  alps\frameworks\base\core\java\android\hardware\SerialPort.java
  ```

- JNI定义

  ```code
  alps\frameworks\base\core\jni\android_hardware_SerialPort.cpp
  ```

## 测试方法

- 1.)确认串口号，波特率，校验方式，停止位等所需的设置是否正确
- 2.)短接TX和RX看是否可以进行自发自收
- 3.)如果是连接电脑通过串口调试工具/超级终端调试，请确认串口线是否可以正常接收，发送数据。可以短接，看能不能自发自收。

## 硬件焊线
  
  硬件焊出TX RX GND三根线连接外围通信设备，用以通信调试。

## 串口log打印方法

- 1）插入UART线后，电脑会自动安装对应驱动，我的电脑-右键-设备管理器-端口：USB-to-Serial Comm Port(COM14)，在此可以查询到UART对应的电脑端口号；
- 2) 串口打印工具
  - a. 电脑自带的超级终端：使用方法："开始"菜单-所有程序-附件-通讯-超级终端-弹出框中填写 名称&端口号，可以自动生成应用弹出界面，可以用来显示打印UART串口信息:
  - b）“串口工具”：网上下载
  - c) SecureCRT：网上下载，使用方法：

## andori uart log eng版本也无法吐出的分析

- 通过修改proc参数的方式： adb shell echo 1 > /proc/mtprintk
- 通过修改Property的方式：adb shell setprop persist.uartconsole.enable 1 （通过getprop persist.uartconsole.enable可查询属性值）
- 修改mt_printk.c文件

  ```c
  void set_uartlog_status(bool value)
  {
  #ifdef CONFIG_MTK_ENG_BUILD
  -       printk_disable_uart = value ? 0 : 1;
  +       printk_disable_uart = value ? 0 : 0;
          pr_info("set uart log status %d.\n", value);
  +#else
  +    printk_disable_uart = value ? 1 : 0;
  +    pr_info("set uart log status %d.\n", value);
  #endif
  }

  @@ -96,7 +99,11 @@ void mt_disable_uart(void)
  {
          /* uart print not always enable */
          if ((mt_need_uart_console != 1) && (printk_disable_uart != 2))
  -               printk_disable_uart = 1;
  +#ifdef CONFIG_MTK_ENG_BUILD
  +               printk_disable_uart = 0;
  +#else
  +        printk_disable_uart = 1;
  +#endif
  }
  ```

## UART驱动代码分析

  参考文档 [MTK UART驱动代码分析](https://blog.csdn.net/dlijun/article/details/50617445)
