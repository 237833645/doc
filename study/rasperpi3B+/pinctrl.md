# 树莓派内核PINCTRL学习

主要作用：

1. 管理系统中所有的可以控制的pin，在系统初始化的时候，枚举所有可以控制的pin，并标识这些pin。
2. 管理这些pin的复用（Multiplexing）。对于SOC而言，其引脚除了配置成普通的GPIO之外，若干个引脚还可以组成一个pin group，行程特定的功能。pin control subsystem要管理所有的pin group。
3. 配置这些pin的特性。例如使能或关闭引脚上的pull-up、pull-down电阻，配置引脚的driver strength。

普通driver调用pin control subsystem的主要目标有两个：

1. 设定该设备的功能复用
2. 设定该device对应的pin的电气特性

## PINCTRL相关内核宏定义

```makefile
CONFIG_PINCTRL=y
    drivers/Makefile:15:obj-$(CONFIG_PINCTRL)               += pinctrl/
    drivers/base/Makefile:22:obj-$(CONFIG_PINCTRL)          += pinctrl.o
    drivers/gpio/gpiolib.c:686:#ifdef CONFIG_PINCTRL
    drivers/gpio/gpiolib-of.c:922:#ifdef CONFIG_PINCTRL
    drivers/gpio/gpiolib.h:61:#ifdef CONFIG_PINCTRL
CONFIG_PINMUX=y
    drivers/pinctrl/Makefile:7:obj-$(CONFIG_PINMUX)         += pinmux.o
    drivers/pinctrl/pinmux.h:12:#ifdef CONFIG_PINMUX
    drivers/pinctrl/core.h:165: #ifdef CONFIG_PINMUX
CONFIG_PINCONF=y
    drivers/pinctrl/Makefile:8:obj-$(CONFIG_PINCONF)        += pinconf.o
    drivers/pinctrl/pinconf.h:13:#ifdef CONFIG_PINCONF    
CONFIG_GENERIC_PINCONF=y
    drivers/pinctrl/Makefile:10:obj-$(CONFIG_GENERIC_PINCONF)       += pinconf-generic.o
    drivers/pinctrl/pinconf.h:101:#if defined(CONFIG_GENERIC_PINCONF) && defined(CONFIG_DEBUG_FS)
    drivers/gpu/drm/tegra/dpaux.c:59:#ifdef CONFIG_GENERIC_PINCONF
# CONFIG_DEBUG_PINCTRL is not set
CONFIG_I2C_MUX_PINCTRL=m
    drivers/i2c/muxes/Makefile:15:obj-$(CONFIG_I2C_MUX_PINCTRL)     += i2c-mux-pinctrl.o
CONFIG_PINCTRL_MCP23S08_I2C=m
    drivers/pinctrl/Makefile:24:obj-$(CONFIG_PINCTRL_MCP23S08_I2C)  += pinctrl-mcp23s08_i2c.o
CONFIG_PINCTRL_MCP23S08_SPI=m
    drivers/pinctrl/Makefile:25:obj-$(CONFIG_PINCTRL_MCP23S08_SPI)  += pinctrl-mcp23s08_spi.o
CONFIG_PINCTRL_MCP23S08=m
    drivers/pinctrl/Makefile:26:obj-$(CONFIG_PINCTRL_MCP23S08)      += pinctrl-mcp23s08.o
# CONFIG_PINCTRL_SINGLE is not set
# CONFIG_PINCTRL_SX150X is not set
# CONFIG_PINCTRL_STMFX is not set
# CONFIG_PINCTRL_OCELOT is not set
CONFIG_PINCTRL_BCM2835=y
    drivers/pinctrl/bcm/Makefile:5:obj-$(CONFIG_PINCTRL_BCM2835)            += pinctrl-bcm2835.o
```
