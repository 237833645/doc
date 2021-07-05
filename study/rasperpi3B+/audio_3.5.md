# 3.5耳机功能

树莓派默认是没有开启audio功能的

## 开启3.5耳机功能

在/boot/firmware/config.txt中加入  dtparam=audio=on 后重启

```code
1. /boot/firmware/config.txt文件中加入
  dtparam=audio=on

2. sudo reboot

3. sudo raspi-config  查看 audio 配置
Headphones

```

## 消除噪音

如果音响输出有噪声，可以尝试树莓派Audio的pwm模式。 /boot/firmware/config.txt

```code
dtparam=audio=on          打开audio总开关
# audio_pwm_mode=2         耳机输出声音
# hdmi_drive=2            hdmi输出声音
```

## 安装mplayer mplayer如何在后台播放的问题

```code
如果要让mplayer在后台播放mp3的话，mplayer * &是不行的，而一定要mplayer * < /dev/null &，原因描述如下：
&是后台运行的标志。dev/null是一个文件，这个文件比较特殊，所有传给它的东西它都丢弃掉。所以这个一般是用来清除回显内容的设备。
mplayer * < /dev/null &就是让mplayer不输出任何信息地在后台运行。
事实上，完整的应该是 mplayer * < /dev/null > /dev/null 2>1&，把 mplayer *产生的所有的标准输出/输入都丢掉，包括标准错误输出(2>1&)，
这样即使出错也不会跳出任何提示，完全安静的后台运行。至于< > 2>1这些重定向随便一个描述shell编程的教材里面都描述得很详细了。
```

```code
mplayer http://lhttp.qingting.fm/live/398/64k.mp3  (读书)

```

## 访问流媒体  https://tingfm.com/

```code
mplayer https://lhttp.qingting.fm/live/386/64k.mp3 

报错如下：
MPlayer 1.3.0 (Debian), built with gcc-7 (C) 2000-2016 MPlayer Team

Playing https://lhttp.qingting.fm/live/386/64k.mp3.
libavformat version 57.83.100 (external)
[https @ 0xffffac606a18]HTTP error 403 Forbidden
Failed to open https://lhttp.qingting.fm/live/386/64k.mp3.


Exiting... (End of file)


修改 https 为http 即可播放
```
