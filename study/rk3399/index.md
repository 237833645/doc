# RK3399 android 7.0平台学习

## 挖掘机 + 蓝宝石 评估板介绍

[Rockchip RK3399挖掘机用户使用指南_V3.0_20190305.pdf](res/Rockchip RK3399挖掘机用户使用指南_V3.0_20190305.pdf)

## RK3399 SDK基础软件包

从瑞芯微拿回来的压缩包是SDK的基础软件包
目录RK3399-PX30下有3个SDK版本
1. PX30_LINUX_SDK_V1.1
2. RK3399_ANDROID7.1-Industry-SDK_V1.0（我目前使用这个）
3. rk3399_ANDROID9.0_V2.0_20190220

标注：基础版本是可以编译也是可以刷机的，串口LOG显示开启正常，但是android投屏软件是不显示的，瑞芯微回复，需要同步SDK代码

## SDK基础软件包和瑞芯微同步更新代码

需要先联系瑞芯微商务，申请KEY参考文档如下：
[Rockchip SDK Kit申请表.docx](./res/Rockchip SDK Kit申请表.docx)
[公钥替换注意事项---2018.docx](./res/公钥替换注意事项---2018.docx)

- Rockchip SDK Kit申请表.docx 填写规则

SDK申请分为2种状态：
1. 没有KEY和SDK包  					---> 生成KEY，填写 "申请开通的SDK版本号" 这栏 、"是否是替换新公钥" 否
2. 有SDK包和KEY，申请其他SDK包  	---> 填写 "已开通的SDK版本号"与 "申请开通的SDK版本号"、"是否是替换新公钥" 是

特殊情况遗忘KEY需要重新申请，联系商务解决

- SSH 公钥操作说明

ssh-keygen -t rsa -C "user@host"  请将 user@host 替换成您的邮箱地址。
请妥善保存生成的私钥文件 id_rsa 和密码，并将 id_rsa.pub 发邮件给 SDK 发布服务器的管理员。

重命名id_rsa.pub为rinlink.pub

- 将rinlink.pub文件和“Rockchip SDK Kit申请表.docx” 一起发给瑞芯微业务申请

## 申请完毕后更新代码

.repo/repo/repo sync -l
.repo/repo/repo sync

这样最新的代码就被同步下来了

## 平台代码编译和下载（瑞芯微把工具文档放在了代码目录里面）

- 编译参考文档 rk3399_android7/RKDocs/rk3399/Rockchip_RK3399_Developer_Guide_Android7.1_Software_CN&EN.pdf
- 下载工具下载文档 rk3399_android7/RKTools/windows/AndroidTool/AndroidTool_Release_v2.65/Android开发工具手册_v1.2.pdf
- 下载工具	rk3399_android7/RKTools/windows/AndroidTool/AndroidTool_Release_v2.65/AndroidTool.exe

