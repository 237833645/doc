# dts

## 解析dtb到dts

```code
cp /boot/firmware/ boot/firmware/bcm2710-rpi-3-b-plus.dtb
dtc -I dtb -O dts -o bcm2710-rpi-3-b-plus.dts bcm2710-rpi-3-b-plus.dtb
```

## dtbo (dtb overlay)

