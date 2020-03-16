# Ubuntu 18.04编译内核学习

## 一、前期准备
1. 安装server版本Ubuntu 18.04（略）
2. 设置系统时间 （略）
3. 更换官方源（略）
4. 查找Ubuntu版本内核版本，查找官方linux内核包 
    ```
    sudo apt-cache search linux-image-*
    ```
5. 下载内核对应的版本
    ```
    sudo apt-get install linux-source-5.3.0
    ```
6. 在/usr/src/目录里面解压开内核代码
    ```
    sudo tar -jxvf linux-source-5.3.0.tar.bz2 -C ../
    ```
## 二、编译内核
- 编译前需要安装的额外软件包支持
    ```
    sudo apt-get install libncurses-dev flex bison libssl-dev
    ```
- 配置内核config文件
    - 可以使用make menuconfig新配置
    - 或者使用系统自带的配置文件来 /boot/config-5.3.0-40-generic (注意，使用这个方案就不要走make menuconfig了)
        ```
        sudo cp /boot/config-5.3.0-40-generic /usr/src/linux-source-5.3.0 .config
        ```
- 编译
    ```
    sudo make mrproper  (如果不是第一次编译内核需执行)
    sudo make menuconfig
    sudo make -j4
    sudo make modules
    sudo make install
    sudo mkinitramfs -ko initrd.img-5.4.7 5.4.7
    sudo update-grub
    sudo reboot
    ```

## 三、替换ubuntu内核

## 四、编译内核模块测试

## 五、总结
- 目前可以在不用linux内核源码编译换内核就可以直接做简单的内核代码编程。ubuntu自带了insmod功能，只需要下载对应系统的内核头文件就可以编译helloworld.ko模块了，具体操作如下：
    ```
    sudo apt-get install linux-headers-$(uname -r)
    ```

