# 学习

## 学习文档总目录

- [STM32-OLED接线](STM32-OLED接线.md)
- [STM32-OLED淘宝](STM32-OLED-taobao.md)
- [ubuntu18.04-内核编译](ubuntu18.04-kernel.md)
- [荔枝派开发板学习文档](LicheePi-zero.md)
- [ESP32开发板]
- [STM32开发板]

## 小技巧

- wget下载整个网站

```bash
wget -c -r -nd -np -k -L -p www.oldlinux.org/Linux.old/
```


- 提升github访问速度

[windows参考如下: https://blog.csdn.net/u013517229/article/details/81351885](https://blog.csdn.net/u013517229/article/details/81351885)

下面的macos无效


```bash
1. sudo vim /etc/hosts

2. # Github
151.101.44.249 github.global.ssl.fastly.net 
192.30.253.113 github.com 
103.245.222.133 assets-cdn.github.com 
23.235.47.133 assets-cdn.github.com 
203.208.39.104 assets-cdn.github.com 
204.232.175.78 documentcloud.github.com 
204.232.175.94 gist.github.com 
107.21.116.220 help.github.com 
207.97.227.252 nodeload.github.com 
199.27.76.130 raw.github.com 
107.22.3.110 status.github.com 
204.232.175.78 training.github.com 
207.97.227.243 www.github.com 
185.31.16.184 github.global.ssl.fastly.net 
185.31.18.133 avatars0.githubusercontent.com 
185.31.19.133 avatars1.githubusercontent.com
192.30.253.120 codeload.github.com


3. sudo killall -HUP mDNSResponder

```