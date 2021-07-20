# Raspberry Pi Zero W

## 下载安装T卡系统 (略)

## T卡boot文件配置

- T卡根目录创建一个ssh的文件
- 修改T卡根目录config.txt和cmdline.txt文件，开启USB Ethernet Gadget功能
  - config.txt最末行处换行添加如下代码，打开usb网卡模式

    ```code
    dtoverlay=dwc2
    ```

  - cmdline.txt文件中找到rootwait字段，并在其后面空格添加如下信息，在打开系统时开启usb网卡模式

    ```code
    modules-load=dwc2,g_ether
    ```

## 配置网络共享

## 登录，配置系统

- ssh pi@raspberrypi.local
- 默认用户名：pi 默认密码raspberry
- ping网络 （更改IP地址）
- 修改网络源

```code
# 编辑 `/etc/apt/sources.list` 文件，删除原文件所有内容，用以下内容取代：
deb http://mirrors.tuna.tsinghua.edu.cn/raspbian/raspbian/ buster main non-free contrib rpi
deb-src http://mirrors.tuna.tsinghua.edu.cn/raspbian/raspbian/ buster main non-free contrib rpi

# 编辑 `/etc/apt/sources.list.d/raspi.list` 文件，删除原文件所有内容，用以下内容取代：
deb http://mirrors.tuna.tsinghua.edu.cn/raspberrypi/ buster main ui
```

- 更新系统
- 下载编译需要的工具

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
sudo apt-get install bc
```

- 下载系统对应的内核代码

```code
cd ~/
sudo apt install rpi-source
./rpi-source
```

- 编译linux内核module代码运行

- 配置samba

```code
1. sudo apt-get install samba-common-bin samba
2. sudo smbpasswd -a pi        #设置samba 账号密码
3. sudo vi /etc/samba/smb.conf
[pi]
path = /home/pi/
available = yes
public = yes
browseable = yes
writeable = yes
valid users = pi
4. sudo service smbd restart   #重启samba服务
```

- 配置NFS
