# S550 MT6739和CUni360S-Z单片机 SPI通信

## 通信方式描述

CUni360S-Z单片机通过SPI3总线获取FM17550的nfc数据（单片机SPI为主）
MT6739通过SPI总线获取CUni360S-Z单片机SPI1的数据(MT6739 SPI为主)

MT6739的SPI电平位是1.8v 单片机是3.3v所以需一个电平转换芯片

## 原理图介绍

- CUni360S-Z单片机到NFC芯片FM17550原理图

- MT6739到CUni360S-Z单片机原理图
