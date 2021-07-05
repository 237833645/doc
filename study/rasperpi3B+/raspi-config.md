# raspi-config

raspi-config 一般在树莓派官方系统是预装的，而在kali、ubuntu mate、osmc、等第三方系统中则是没有内置raspi-config的，有时raspi-config的gui设置又相对方便友好，对新手来说能解决很多问题。

## 1、安装依赖

sudo apt install whiptail parted lua5.1 alsa-utils psmisc

## 2、从Raspberrypi官网下载最新的deb安装包

http://archive.raspberrypi.org/debian/pool/main/r/raspi-config/

wget http://archive.raspberrypi.org/debian/pool/main/r/raspi-config/raspi-config_20200707_all.deb

## 3 安装

dpkg -i raspi-config_xxxxxxxx.deb

## 发现有依赖报错，修复依赖

sudo apt --fix-broken install

## 然后重新安装
