# SPI NorFlash xt25f128b

## 背景

在买licheepi zero的时候在淘宝店多买了一个NorFlash，并焊接到主板上了，我买的是16M winbond xt25f128b型号

## NorFlash移植前需要资料收集

- [uboot移植nor-flash](https://blog.csdn.net/juhou/article/details/98075294)
- [荔枝派Nano F1C100s u-boot都正常，挂在根文件时出错了](https://whycan.com/t_4464.html)

本人需要移植的 flash 为 xt25f128b，经过查询数据手册，发现和 winbond w25qxxx 系列的 flash 兼容性很高，硬件特性、指令基本一样于是觉得基于 w25qxxx 系列进行移植

## uboot 移植norflash

CONFIG_SPL_MTD_SUPPORT
CONFIG_SPL_SPI_FLASH_SUPPORT
CONFIG_SPL_BUILD
CONFIG_SPI_FLASH
CONFIG_SPI_FLASH_MTD

```bash

```
