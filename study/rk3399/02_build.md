# 编译Android 7.1 Rk3399

参考文档：《RK3399_ANDROID7.1_行业SDK_V1.0_20180408.pdf》
此文档一般和代码放在同一目录

[RK3399_ANDROID7.1_行业SDK_V1.0_20180408.pdf](./res/RK3399_ANDROID7.1_行业SDK_V1.0_20180408.pdf)

## 挖掘机编译

- uboot 编译:

```code
cd u-boot 
make rk3399_defconfig 
make ARCHV=aarch64

```

- kernel 编译:

```code
cd kernel 
make ARCH=arm64 rockchip_defconfig -j8 
make ARCH=arm64 rk3399-sapphire-excavator-edp.img -j12
```

- android 编译:

```code
source build/envsetup.sh 
lunch rk3399_all-userdebug 
make -j12 
./mkimages.sh
```

完成编译后，执行 SDK 根目录下的 mkimage.sh 后在 rockdev/Image-xxx/目录生成完整
的固件包(xxx 是具体 lunch 的产品名)。

- 编译及打包update.img

```code
source build/envsetup.sh && lunch rk3399_all-userdebug && ./build-rk3399-all.sh -u

```
