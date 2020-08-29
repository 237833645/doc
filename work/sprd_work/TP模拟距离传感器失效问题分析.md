# TP模拟距离传感器失效问题分析

## 问题描述

1. 原始问题现象：
2. 提交CQ给展讯分析，展讯给出修改framework方案，修改代码如下：
3. 回退掉展讯对framework的修改，分析原始问题现象
4. 驱动端临时修改方案

## 分析过程如下

1 原始问题现象：
拨打电话 -> 遮挡距离传感器（正常）-> 按power键灭屏后再按power键亮屏 —> 遮挡距离传感器（失效） （必现）
使用过程中概率性出现遮挡距离传感器失效问题 (出现此问题后无法修复)
2 提交CQ给展讯分析，展讯给出修改framework方案，修改代码如下：

```java
frameworks/base/services/core/java/com/android/server/display/DisplayPowerController.java
          && mProximity == PROXIMITY_POSITIVE
          && state != Display.STATE_OFF) {
          setProximitySensorEnabled(true);
-         } else {
+         } else if (!mTelecomManager.isInCall()) {
        setProximitySensorEnabled(false);
        mWaitingForNegativeProximity = false;
    }
```

此段代码目的为在非通话状态关闭framework对sensor的event处理，并下发关闭距离传感器功能，其他状态都不做处理

此问题修改没有解决掉问题的根源

3 回退掉展讯对framework的修改，分析原始问题现象

分析步骤： 拨打电话 -> 遮挡距离传感器（正常）-> 按power键灭屏后再按power键亮屏 —> 遮挡距离传感器（失效） （必现)   
单步操作抓取log分析过程:   
1）拨打电话 -> 遮挡距离传感器（正常） –抓取log分析，逻辑正常   
2）拨打电话 -> 按power键灭屏后再按power键亮屏 –抓取log分析，逻辑正常   
3）拨打电话 -> 遮挡距离传感器（正常）-> 按power键灭屏后再按power键亮屏 –抓取log分析，逻辑正常   
4）拨打电话 -> 遮挡距离传感器（正常）-> 按power键灭屏后再按power键亮屏 —> 遮挡距离传感器 –抓取log分析，I2C通信失败，log截图如下

```code
[ 3210.150184] c1 ILITEK MsDrvInterfaceTouchDeviceFbNotifierCallback, lind = 14286,*** MsDrvInterfaceTouchDeviceFbNotifierCallback() ***
[ 3210.150195] c1 ILITEK MsDrvInterfaceTouchDeviceFbNotifierCallback, lind = 14391,*** MsDrvInterfaceTouchDeviceFbNotifierCallback() TP Suspend ***
[ 3210.150204] c1 ILITEK MsDrvInterfaceTouchDeviceFbNotifierCallback, lind = 14421, g_GestureState = 0, g_ChipType = 0x7A
[ 3210.150213] c1 ILITEK DrvOpenGestureWakeup, lind = 13416,*** DrvOpenGestureWakeup() ***
[ 3210.150222] c1 ILITEK DrvOpenGestureWakeup, lind = 13420,wakeup mode 0 = 0xffff
[ 3210.150228] c1 ILITEK DrvOpenGestureWakeup, lind = 13421,wakeup mode 1 = 0x0
[ 3210.170839] c1 ILITEK DrvOpenGestureWakeup, lind = 13526,Enable gesture wakeup success
[ 3215.080153] c3 ILITEK _DrvProximityOpen, lind = 3139,*** _DrvProximityOpen() ***
[ 3215.080185] c3 ILITEK _DrvProximityIoctl, lind = 3182,*** _DrvProximityIoctl() *** cmd = 3
[ 3215.080192] c0 ILITEK _DrvProximityIoctl, lind = 3196,flag = 1ILITEK DrvTpPsEnable, lind = 3123,*** DrvTpPsEnable() nEnable = 1 ***
[ 3215.080203] c3 ILITEK DrvEnableProximity, lind = 3015,*** DrvEnableProximity() ***

 [ 3215.101395] c0 i2c i2c-3: i2c report: no ack error !!!
 [ 3215.101412] c0 i2c i2c-3: : ======dump i2c-3 reg=======
 [ 3215.101420] c0 i2c i2c-3: : I2C_CTRL:0xf4106
 [ 3215.101428] c0 i2c i2c-3: : I2C_ADDR_CFG:0x4c
 [ 3215.101436] c0 i2c i2c-3: : I2C_COUNT:0x4
 [ 3215.101444] c0 i2c i2c-3: : I2C_RX:0xff
 [ 3215.101453] c0 i2c i2c-3: : I2C_STATUS:0x210802
 [ 3215.101461] c0 i2c i2c-3: : ADDR_DVD0:0xc0012
 [ 3215.101469] c0 i2c i2c-3: : ADDR_DVD1:0x0
 [ 3215.101477] c0 i2c i2c-3: : ADDR_STA0_DVD:0xf
 [ 3215.101486] c0 i2c i2c-3: : ADDR_RST:0x0
 [ 3215.101596] c3 IicWriteData() error -5
```

系统流程： 从驱动代码抓取LOG分析通话过程中的sensor传感器流程和tp的流程如下:   
屏幕亮着状态来电话遮挡距离传感器此时流程:通话功能触发，framework服务端先下发打开距离传感器功能，framework端开始获取距离传感器状态是否有遮挡状态， 如果有framework端关闭系统背光，如果检测到距离离开，framework端打开系统背光。
屏幕亮着状态来电话按power键灭屏后再按power键亮屏后遮挡距离传感器流程:通话功能触发，framework服务端先下发打开距离传感器功能， framework端开始获取距离传感器状态是否有遮挡状态，按framework端电源管理会直接进入灭屏流程
I2C通信失败原因： 1）通话灭屏分析：通话界面按power键灭屏：通话界面系统会先下发关闭传感器动作DrvDisableProximity()，屏幕一灭后执行了TP端的(*pBlank == DRM_MODE_DPMS_OFF)流程。 由于g_EnableTpProximity状态先被DrvDisableProximity函数修改为0，导致了此流程内部未进入return流程，进而TP进入到了彻底关闭状态

```code
#ifdef CONFIG_ENABLE_PROXIMITY_DETECTION
	if (g_EnableTpProximity == 1)  //此g_EnableTpProximity全局变量在话界面系统会先下发关闭传感器动作DrvDisableProximity()已经更改为0
	{
		bSuspendReturn= 1;
		DBG(&g_I2cClient->dev, "g_EnableTpProximity = %d\n", g_EnableTpProximity);
		return 0;
	}
#endif //CONFIG_ENABLE_PROXIMITY_DETECTION

	DrvFingerTouchReleased(0, 0, 0); // Send touch end for clearing point touch
	input_sync(g_InputDevice);

	DrvDisableFingerTouchReport();
	{
		DrvTouchDevicePowerOff();
#ifdef CONFIG_ENABLE_REGULATOR_POWER_ON
		DrvTouchDeviceRegulatorPowerOn(false);
#endif //CONFIG_ENABLE_REGULATOR_POWER_ON
	}
```

通话后power键灭屏后再按power键唤醒分析： 再按power键唤醒，framework端先使能距离传感器功能，并下发指令到驱动执行DrvEnableProximity()，此时g_EnableTpProximity变量=1，I2C通信对IC进行模式写入，由于上次是TP彻底关闭了， 所以再按power键的唤醒屏幕，在屏幕还没有亮的时候下发了打开距离传感器功能（通过I2C协议写入TP寄存器，更改TP模式为传感器模式），I2C通信失败。强调：此时TP还未工作 代码片段如下：

```code
s32 DrvEnableProximity(void)
{
	u8 szTxData[4] = {0};
	u32 i = 0;
	s32 rc;

	DBG(&g_I2cClient->dev, "*** %s() ***\n", __func__);

	szTxData[0] = 0x52;
	szTxData[1] = 0x00;

	if (g_ChipType == CHIP_TYPE_MSG22XX)
	{
		szTxData[2] = 0x4a;
	}
	else if (g_ChipType == CHIP_TYPE_MSG28XX || g_ChipType == CHIP_TYPE_MSG58XXA || g_ChipType == CHIP_TYPE_ILI2118A || g_ChipType == CHIP_TYPE_ILI2117A)
	{
		szTxData[2] = 0x47;
	}
	else
	{
		DBG(&g_I2cClient->dev, "*** Un-recognized chip type = 0x%x ***\n", g_ChipType);
		return -1;
	}

	szTxData[3] = 0xa0;

	while (i < 5)
	{
		mdelay(I2C_WRITE_COMMAND_DELAY_FOR_FIRMWARE); // delay 20ms
		rc = IicWriteData(SLAVE_I2C_ID_DWI2C, &szTxData[0], 4);//此I2C操作如果TP已经彻底关闭，I2C会报错
		if (rc > 0)
		{
			g_EnableTpProximity = 1;

			DBG(&g_I2cClient->dev, "Enable proximity detection success\n");
			break;
		}

		i++;
	}
	if (i == 5)
	{
		DBG(&g_I2cClient->dev, "Enable proximity detection failed, rc = %d\n", rc);
	}

	return rc;
}	
```

4 驱动端临时修改方案

由于在DrvEnableProximity()时I2C会失败，所以驱动端在TP的resume函数最后再加入一次DrvEnableProximity(),这样的修改可以解决在失效后按power键重新亮屏后能够修复好模拟距离的功能 代码片段如下：

```code
if(1 == g_EnableProximity)
{
		printk("wangfeng g_EnableProximityFail == 1\n");
		DrvEnableProximity();
}
```

驱动端修改无法解掉I2C通信失败导致的失效问题，但是按power键亮屏后再遮挡是可以的修复的
