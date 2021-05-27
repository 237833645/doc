# 荔枝派ZERO使用

## 开发环境搭建

- **Ubuntu**系统安装(略)

  在电脑端安装了Ubuntu 18.04的server虚拟机。

- 下载**Licheepi**编译器及**uboot**、**kernel**、**rootfs**、**tools**、

  - 编译工具链

  ```bash
  sudo apt-get install build-essential libncurses5-dev unzip python
  ```

  ```bash
  wget -c https://releases.linaro.org/components/toolchain/binaries/6.3-2017.05/arm-linux-gnueabihf/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf.tar.xz
  tar xvf gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf.tar.xz
  mv gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf /opt/
  vim /etc/bash.bashrc
  # add: PATH="$PATH:/opt/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf/bin"
  source /etc/bash.bashrc
  arm-linux-gnueabihf-gcc -v
  sudo apt-get install device-tree-compiler
  ```

  - uboot下载编译（编译完成后，在当前目录下生成了u-boot-sunxi-with-spl.bin，可以烧录到8K偏移处启动）
  
  ```bash
  git clone https://github.com/Lichee-Pi/u-boot.git -b v3s-current
  #or git clone https://github.com/Lichee-Pi/u-boot.git -b v3s-spi-experimental (我使用这个，这个是支持spi-norflash的)
  cd u-boot
  make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- LicheePi_Zero_800x480LCD_defconfig
  #or make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- LicheePi_Zero480x272LCD_defconfig
  #or make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- LicheePi_Zero_defconfig  （(我使用这个，不支持LCD）
  make ARCH=arm menuconfig
  time make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- 2>&1 | tee build.log
  ```

  - uboot spi-norflash支持patch

  ```bash
  ....
  ```

  - linux内核下载 zero-4.10.y (编译完成后，zImage在arch/arm/boot/下，驱动模块在out/下)

  ```bash
  git clone https://github.com/Lichee-Pi/linux.git
  cd linux
  make ARCH=arm licheepi_zero_defconfig
  make ARCH=arm menuconfig   #add bluethooth, etc.
  make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j16
  make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j16 INSTALL_MOD_PATH=out modules
  make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j16 INSTALL_MOD_PATH=out modules_install
  ```

  - rootfs下载

  ```bash
  wget https://buildroot.org/downloads/buildroot-2017.08.tar.gz
  tar xvf buildroot-2017.08.tar.gz
  cd buildroot-2017.08/
  make menuconfig
  make
  ```

---

## uboot调试编译

## kernel调试编译

## rootfs调试编译

## 工具使用
