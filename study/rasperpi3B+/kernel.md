# 内核编译及内核module使用

## 内核对应的源码

```bash
cd /usr/src/
wget -c https://github.com/raspberrypi/linux/archive/refs/heads/rpi-5.10.y.zip
sudo unzip rpi-5.10.y.zip
```

## 创建内核源码软连接

```bash
sudo ln -s /usr/src/linux-rpi-5.10.y /lib/modules/`uname -r`/build
```

## 清理内核代码

```bash
cd /lib/modules/`uname -r`/build
sudo make mrproper
```

## 使用当前系统的config来配置内核

查看 /proc/config.gz文件是否存在，如果不存在，使用下面命令生成

```bash
sudo modprobe configs
```

```bash
zcat /proc/config.gz > .config
```
