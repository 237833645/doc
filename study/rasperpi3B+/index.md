# 树莓派3B+ ubuntu server

- [树莓派3B+ ubuntu server](#树莓派3b-ubuntu-server)
  - [系统安装步骤 （下载回来的文件较大放入移动硬盘中）](#系统安装步骤-下载回来的文件较大放入移动硬盘中)
  - [插入hdmi+键盘+网线开机](#插入hdmi键盘网线开机)
  - [配置](#配置)
  - [配置 wifi](#配置-wifi)
  - [配置 BT](#配置-bt)

---

## 系统安装步骤 （下载回来的文件较大放入移动硬盘中）

- 格式化T卡工具 “SDCardFormatterv5_WinEN.zip”
- 刷入镜像工具 “balenaEtcher-Setup-1.5.115.exe”
- ubuntu镜像源 “ubuntu-18.04.5-preinstalled-server-arm64+raspi3.img” （最新的ubuntu版本开机有问题）
- ubuntu server 系统默认的用户名为：ubuntu ，密码为：ubuntu

## 插入hdmi+键盘+网线开机

---

## 配置

- 更换源 中国科学技术大学
  
  Raspbian http://mirrors.ustc.edu.cn/raspbian/raspbian/

- 内核安装

```code
sudo apt-get install linux-image-`uname -r`
sudo apt-get install linux-headers-`uname -r`
```

- 工具安装

```code
sudo apt-get install openssh-server
sudo apt-get install build-essential 
sudo apt-get install make
sudo apt-get install gcc
sudo apt-get install bison
sudo apt-get install libncurses5-dev
sudo apt-get install openssl
sudo apt-get install libssl-dev
sudo apt-get install git
sudo apt-get install ntpdate

```

- 设置时区

```code

```

---

## 配置 wifi

- 1. 安装wifi工具

```code
sudo apt install wireless-tools
```

- 2. 首先设置打开无线网卡并设置SSID

```code
sudo iwconfig wlan0 power on
sudo ifconfig wlan0 up
sudo iwconfig wlan0 essid "无线名称"
sudi iwconfig
```

- 3. 使用wpa_passphrase生成无线密钥

```code
sudo -i
wpa_passphrase 无线名称 密码 > /etc/wpa_supplicant.conf
```

- 4. 接下来编辑 /etc/wpa_supplicant.conf ,确保文件内容如下

```code
ctrl_interface=/var/run/wpa_supplicant
ap_scan=1
network={
        ssid="007-2"
        #psk="LLzn8355"
        psk=72e65c854061dc9d91232e90f62ce70016bcbf2a53e90875ffd0a38fd51c8b72
}
```

- 5. 以上配置文件设置完成后，接下来手动应用配置

```code
sudo wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant.conf
sudo dhclient wlan0
```

- 6. 为了开机自动开启wifi，需要将启动脚本加到自动执行服务中，首先在/etc/rc.local中加入如下脚本：

```code
#!/bin/bash
ip link set wlan0 up &&
wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant.conf &&
dhclient wlan0
exit 0
```

并修改rc.local为可执行
  
```code
chmod a+x /etc/rc.local
```

- 7. 然后编辑 /lib/systemd/system/rc-local.service加入如下配置：

```code
[Unit]
Description=/etc/rc.local Compatibility
Documentation=man:systemd-rc-local-generator(8)
ConditionPathExists=/etc/rc.local
After=network.target

[Service]
Type=forking
ExecStart=/etc/rc.local start
TimeoutSec=0
RemainAfterExit=yes
GuessMainPID=no

[Install]
WantedBy=multi-user.target
Alias=rc-local.service
```

- 8. 设置开机默认启动rc-local.service

执行下面命令后自动在/etc/systemd/system下创建rc-local.service到 /lib/systemd/system/rc-local.service的软链接。

```code
systemctl enable rc-local.service
```

- 9. 最后验证下服务能否正常开启运行

```code
systemctl start rc-local.service &&
system status rc-local.service
```

- 10. 如果出现下面信息，就ok了，这样就可以实现开机自动连接wifi了。

```code
● rc-local.service - /etc/rc.local Compatibility
   Loaded: loaded (/lib/systemd/system/rc-local.service; enabled; vendor preset: enabled)
  Drop-In: /lib/systemd/system/rc-local.service.d
           └─debian.conf
   Active: active (exited) since Wed 2021-06-30 20:24:54 CST; 13s ago
     Docs: man:systemd-rc-local-generator(8)
  Process: 3520 ExecStart=/etc/rc.local start (code=exited, status=0/SUCCESS)

Jun 30 20:24:54 ubuntu systemd[1]: Starting /etc/rc.local Compatibility...
Jun 30 20:24:54 ubuntu rc.local[3520]: Successfully initialized wpa_supplicant
Jun 30 20:24:54 ubuntu rc.local[3520]: ctrl_iface exists and seems to be in use - cannot override it
Jun 30 20:24:54 ubuntu rc.local[3520]: Delete '/var/run/wpa_supplicant/wlan0' manually if it is not used anymore
Jun 30 20:24:54 ubuntu rc.local[3520]: Failed to initialize control interface '/var/run/wpa_supplicant'.
Jun 30 20:24:54 ubuntu rc.local[3520]: You may have another wpa_supplicant process already running or the file was
Jun 30 20:24:54 ubuntu rc.local[3520]: left by an unclean termination of wpa_supplicant in which case you will need
Jun 30 20:24:54 ubuntu rc.local[3520]: to manually remove this file before starting wpa_supplicant again.
Jun 30 20:24:54 ubuntu rc.local[3520]: nl80211: deinit ifname=wlan0 disabled_11b_rates=0
Jun 30 20:24:54 ubuntu systemd[1]: Started /etc/rc.local Compatibility.
```

- 11. 检查防火墙

```code
sudo ufw status numbered     // 检查防火墙状态(显示规则编号) 
sudo ufw allow samba     // 允许所有的外部IP访问本机的samba端口 
sudo ufw allow ssh         // 允许所有的外部IP访问本机的samba端口(22)
```

| 网络名称 | 网络地址 |
| :-    | -:     |
| 公司网络WIFI | ubuntu@192.168.3.67 |
| 公司网络LAN | ubuntu@192.168.1.139 |

---

## 配置 BT

- xxx

---