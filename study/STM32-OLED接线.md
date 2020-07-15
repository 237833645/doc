# STM32 OLED接线调试过程

## STM32到OLED端接线

|STM32|OLED|
|-|-|
|GND| GND  |
|VCC3.3   |  VCC |
|PA5   |  D0 |
|PA6   | D1  |
|PA1   | RES  |
|PA2   |  DC |
|PA4   |  CS |

## 代码调试过程

- platformio.ini配置依赖库（需要"Adafruit GFX Library"和"Adafruit SSD1306"）

```c
[env:genericSTM32F103C8]
platform = ststm32
board = genericSTM32F103C8
framework = arduino
upload_protocol = stlink
debug_tool = stlink

lib_deps =
    Adafruit GFX Library
    13
    Adafruit GFX Library@1.5.4

    Adafruit SSD1306
    135
    Adafruit SSD1306@1.2.9
```

- 修改编译报错(.piolibdeps/Adafruit SSD1306_ID135/Adafruit_SSD1306.h)

```c
#define SSD1306_128_64 ///< DEPRECTAED: old way to specify 128x64 screen
//#define SSD1306_128_32   ///< DEPRECATED: old way to specify 128x32 screen
//#define SSD1306_96_16  ///< DEPRECATED: old way to specify 96x16 screen
```

- 修改demo程序里面的管脚定义

```c
#define OLED_MOSI  PA6
#define OLED_CLK   PA5
#define OLED_DC    PA2
#define OLED_CS    PA4
#define OLED_RESET PA1
```
