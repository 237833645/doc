# MTK gps配置

##查看原理图：

```c
GPIO_GPS_LNA_EN // 据此查到对应GPIO79(AL27)
```

## 修改dws\dts

```c
/* CONSYS GPIO standardization */
&pio {
    consys_pins_default: default {
};
gpslna_pins_init: gpslna@0 {
    pins_cmd_dat {
    pins = <PINMUX_GPIO79__FUNC_GPIO79>;
    slew-rate = <0>;
    bias-disable;
    output-low;
    };
};

gpslna_pins_oh: gpslna@1 {
    pins_cmd_dat {
        pins = <PINMUX_GPIO79__FUNC_GPIO79>;
        slew-rate = <1>;
        output-high;
    };
};

gpslna_pins_ol: gpslna@2 {
    pins_cmd_dat {
        pins = <PINMUX_GPIO79__FUNC_GPIO79>;
        slew-rate = <1>;
        output-low;
        };
    };
};

&consys {
    pinctrl-names = "default", "gps_lna_state_init", "gps_lna_state_oh", "gps_lna_state_ol";
    pinctrl-0 = <&consys_pins_default>;
    pinctrl-1 = <&gpslna_pins_init>;
    pinctrl-2 = <&gpslna_pins_oh>;
    pinctrl-3 = <&gpslna_pins_ol>;
    status = "okay";
};
```

## 由省gps晶体，改为不省gps晶体

```c
1. 修改alps\device\mediatek\mt6580\WMT_SOC.cfg
- co_clock_flag=1 // 默认为省晶体
+ co_clock_flag=0
```

## 修改CFG_GPS_Default.h文件

```c
ap_nvram_gps_config_struct stGPSConfigDefault =
{
/* if chip detector say it's not 3332 use /dev/stpgps,else use /ttyMT1 */
{'/','d','e','v','/','s','t','p','g','p','s',0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
/* 0:s/w, 1:none, 2:h/w */
1,
/* 26MHz */
26000000,
/* default is 0ppm, by chip definetion. 6620 is 500, else 2000 */
0,
/* 0:16.368MHz TCXO */
- 0xFE, // 默认为0xFF，省晶体要配为0xFE
+ 0xFF, // 默认为0xFF，不省晶体要配为0xFF
```

## 更改modem配置-重新配modem（略）

## 小结

- MTK软件默认是开启了如下4种卫星定位： 北斗、GPS、伽利略、格洛纳斯
- 调试时需要量测gps的clock时钟
