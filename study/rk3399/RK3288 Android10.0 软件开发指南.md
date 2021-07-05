# RK3288 Android10.0 软件开发指南

## 编译

```code
source javaenv.sh
source build/envsetup.sh
lunch rk3399_Android10-userdebug
./build.sh -UKApu -v=userdebug -J=64
```

## 打包 update.img报错

经过分析是afptool 这个工具是需要更高版本的libstdc++.so.6才能使用，我们的ubuntu系统版本是 “Ubuntu 14.04.5 LTS”

查看本地/usr/lib/x86_64-linux-gnu/libstdc++.so.6版本

```code
$ strings /usr/lib/x86_64-linux-gnu/libstdc++.so.6 | grep GLIBCXX
GLIBCXX_3.4
GLIBCXX_3.4.1
GLIBCXX_3.4.2
GLIBCXX_3.4.3
GLIBCXX_3.4.4
GLIBCXX_3.4.5
GLIBCXX_3.4.6
GLIBCXX_3.4.7
GLIBCXX_3.4.8
GLIBCXX_3.4.9
GLIBCXX_3.4.10
GLIBCXX_3.4.11
GLIBCXX_3.4.12
GLIBCXX_3.4.13
GLIBCXX_3.4.14
GLIBCXX_3.4.15
GLIBCXX_3.4.16
GLIBCXX_3.4.17
GLIBCXX_3.4.18
GLIBCXX_3.4.19
GLIBCXX_DEBUG_MESSAGE_LENGTH


$ strings /usr/lib/x86_64-linux-gnu/libstdc++.so.6 | grep CXXABI
CXXABI_1.3
CXXABI_1.3.1
CXXABI_1.3.2
CXXABI_1.3.3
CXXABI_1.3.4
CXXABI_1.3.5
CXXABI_1.3.6
CXXABI_1.3.7
CXXABI_TM_1

```

```code
Make update.img
start to make update.img...
./afptool: /usr/lib/x86_64-linux-gnu/libstdc++.so.6: version `GLIBCXX_3.4.20' not found (required by ./afptool)
./afptool: /usr/lib/x86_64-linux-gnu/libstdc++.so.6: version `CXXABI_1.3.9' not found (required by ./afptool)
./afptool: /usr/lib/x86_64-linux-gnu/libstdc++.so.6: version `GLIBCXX_3.4.21' not found (required by ./afptool)
Press any key to quit:

```

## 挖掘机开发版android10.0功能测试问题

| 功能名称 | 测试结果 | 备注 |
| :-      | :-     | :-:  |
| rj45  | ok |  |
| hdmi输出显示 | ok |   |
| usb | ok |   |
| usb-hub | ok | |
| 按键  | ok |   |
| sdcard | ok | |
| 恢复出厂设置 | ok | |
| 耳机mic | fail | |
| 耳机声音 | ok | |
| 板载mic  | fail | |
| 板载speaker  | 未测试 | 没有设备 |
| wifi | ok |  |
| bt | ok |  |
| usb-uart | ok | |
| hdmi输入  | 未测试 | 没有测试APK和设备 |
| 红外      | 未测试 | 没有测试APK |
| 充电      | 未测试 | 没有电池设备 |
| 电池      | 未测试 | 没有设备 |
| 投屏      | 横屏显示花屏 |（hdmi界面显示正常） |
| pcie      | 未测试 | 没有设备 |
| GPS       | 不支持 | 没有设备 |
| SIM卡      | 不支持 | 没有设备 |
| edp       | 未测试 | 没有设备 |
| mipi显示   | 未测试 | 没有设备 |
| mipi摄像头  |未测试 | 没有设备 |
| TP         | 未测试 | 没有设备 |
| 重力传感器   | ok | |
| 磁力传感器   | fail | |
| 光距离传感器 | fail |  |
| HALL传感器 | 未测试 | 没有APK |
| 陀螺仪 |    | fail |  |

## 