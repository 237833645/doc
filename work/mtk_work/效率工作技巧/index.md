# 效率工作技巧

## 零零PDF密码

2762463240

---

## adb 操作技巧

- adb 睡眠唤醒系统

```bash
系统睡眠            adb shell echo mem > /sys/power/state
系统唤醒            adb shell echo  on > /sys/power/state
```

- adb 开关LCD背光灯

```bash
关LCD背光灯         adb shell echo  0 > sys/class/leds/lcd-backlight/brightness
开LCD背光灯         adb shell echo  255 > sys/class/leds/lcd-backlight/brightness
```

- adb获取event消息

```bash
获取内核按键名    adb shell getevent -l
获取内核按键值    adb shell getevent
获取app的按键名   adb shell logcat | grep KeyEvent
```

- adb 发送power键值

```bash
发送power键         adb shell input keyevent 26
```

- adb 开启关闭MTKLog

```bash
开启MTKLog          adb shell am broadcast -a com.mediatek.mtklogger.ADB_CMD -e cmd_name start --ei cmd_target 7
关闭MTKLog          adb shell am broadcast -a com.mediatek.mtklogger.ADB_CMD -e cmd_name stop  --ei cmd_target 7
```

- adb 开启关闭飞行模式

```bash
打开飞行模式        adb shell settings put global airplane_mode_on 1
关闭飞行模式        adb shell settings put global airplane_mode_on 0
```

- adb 设置屏幕分辨率及dpi

```bash
读取屏幕分辨率      adb shell wm size
设置屏幕分辨率      adb shell wm size 1136x2480
恢复为默认分辨率    adb shell wm size reset

读取屏幕DPI         adb shell wm density
设置屏幕DPI         adb shell wm density 480
恢复屏幕DPI         adb shell wm desity reset
```

- 开启系统自带返回键

```bash
adb root
adb shell setprop qemu.hw.mainkeys 0
adb shell stop
adb shell start
```

- 打开拨号盘

```bash
adb shell am start -n com.android.dialer/com.android.dialer.DialtactsActivity
```

- 打开设置界面

```bash
adb shell am start -n com.android.settings/com.android.settings.Settings$ApnSettingsActivity
```

- adb 读取adc值

## 工程编译及技巧

- 克隆工程

```bash
克隆39工程
1. perl vendor/mediatek/proprietary/scripts/project_clone/project_clone.pl -p "/home/rinlink/rinlink/platform_o1_6739" -o "mediateksample/h900_1g" -n "mediateksample/h900_512"
2. dws文件软链接
3. lk lcm-4.4makefile相关文件修改
4. 对比文件目录差异
```

- 编译技巧

```bash
6739O平台编译   odmdtboimage bootimage
6737N平台编译   dtboimage bootimage
```
