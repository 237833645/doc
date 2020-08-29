# mtk电池驱动流程详解

文档参考链接地址:[mtk 电池驱动流程详解](https://blog.csdn.net/mrdongshiyi/article/details/85104839)

mtk 电池驱动流程详解
充电算法，充9S停1S
电池温度高于50，充电器电压为>6.5V 停止充电，
充电电压最大值是6500mV 最小值是4400mV 3.4V为开机电压，电压大于3.4V才能开机

```c
// 获得 BATSNS 引脚电压 
bat_vol = battery_meter_get_battery_voltage(KAL_TRUE);
/* 获得 PMIC 的  ISENSE 引脚电压*/
Vsense = battery_meter_get_VSense();
if (upmu_is_chr_det() == KAL_TRUE)
    /* 获得充电电流 */
    ICharging = battery_meter_get_charging_current();
else
    ICharging = 0;

//获得充电信息
/* 获得充电器电压 */
charger_vol = battery_meter_get_charger_voltage();
/* 通过获得当前 NTC 电压，查表并进行线性插值法，得到当前的温度值  */
temperature = battery_meter_get_battery_temperature();
 /* 这里用来获取电池 NTC 的电压 */
temperatureV = battery_meter_get_tempV();
/* 获得 下拉电阻与 NTC 并并联的电压 */
temperatureR = battery_meter_get_tempR(temperatureV);


 /* 获得充电电流 */


  BMT_status.ICharging = mt_battery_average_method(&batteryCurrentBuffer[0],ICharging, &icharging_sum, batteryIndex);
   // 获得 BATSNS 引脚电压
  BMT_status.bat_vol = mt_battery_average_method(&batteryVoltageBuffer[0],bat_vol, &bat_sum, batteryIndex);
 /* 通过获得当前 NTC 电压，查表并进行线性插值法，得到当前的温度值  */
  BMT_status.temperature = mt_battery_average_method(&batteryTempBuffer[0],temperature, &temperature_sum, batteryIndex);
  BMT_status.Vsense = Vsense;/* 获得 PMIC 的  ISENSE 引脚电压*/
  BMT_status.charger_vol = charger_vol;  /* 获得充电器电压 */
  BMT_status.temperatureV = temperatureV;/* 这里用来获取电池 NTC 的电压 */
  BMT_status.temperatureR = temperatureR;/* 获得 下拉电阻与 NTC 并并联的电压 */
  BMT_status.SOC = SOC;
  BMT_status.ZCV = ZCV;//开路电压
  BMT_status.CURRENT_NOW = BMT_status.IBattery;//当前充电电流

  /* bat_thread_wakeup() 每 10s 唤醒一次，唤醒时设置  bat_meter_timeout = KAL_TRUE
  这时候更新电池电量百分比 */
  
  /* oam 算法通过两种方式更新电压，去逼近真实的开路电压，最终查表获取近似真实的电量值百分比，方法 1，查表获得电池百分比，方法 2，库伦积分
 以方法2获得的参数补偿方法 1 的值，具体方法见 oam_run()*/
  //6582 平台用的计量方法【在 Battery_Charging_Introduction_for_customer_V1.0.pdf】
  //SW FG算法和HW FG算法。事实上MTK平台项目通常采用的是混合型算法。

  /* SW FG的核心 在于 通过两种方式更新电压，去逼近真实开路电压  最终查表获取近似真实的电 量值。
  ocv1 被假定为开路电压  
  ocv2则是闭路电压，
   D0 D1 D2 D3 D4 D5 代表不同的放电深度*/


 /* 通过当前开路电压 - 负载时电压 = 当前电流 * 电池内阻
获得当前电流值 */
oam_i_1 = (((oam_v_ocv_1 - vol_bat) * 1000) * 10) / oam_r_1;        /* 0.1mA */
oam_i_2 = (((oam_v_ocv_2 - vol_bat) * 1000) * 10) / oam_r_2;        /* 0.1mA */

  /* 当前的变化电量值 = 当前电流 * 上次运行此函数到现在的时间  + 上次的电量值  */
oam_car_1 = (oam_i_1 * delta_time / 3600) + oam_car_1;      /* 0.1mAh */
oam_car_2 = (oam_i_2 * delta_time / 3600) + oam_car_2;      /* 0.1mAh */

    /* 这里使用的就是库伦积分法:
 D1 = D0 + (-CAR)/Cmax
获得当前的电池容量百分比 */
oam_d_1 = oam_d0 + (oam_car_1 * 100 / 10) / gFG_BATT_CAPACITY_aging;
if (oam_d_1 < 0)
    oam_d_1 = 0;
if (oam_d_1 > 100)
    oam_d_1 = 100;

oam_d_2 = oam_d0 + (oam_car_2 * 100 / 10) / gFG_BATT_CAPACITY_aging;
if (oam_d_2 < 0)
    oam_d_2 = 0;
if (oam_d_2 > 100)
    oam_d_2 = 100;

```

整个程序的核心在这里, 他使用了两种方法更新电量:

1. 使用补偿过的闭路电压，查表获得电量 【返回给用户的】
2. 使用软件库伦积分，得到电量值 【用来校正的】

两个方法相对独立，但是在此处，方法 1 使用了 方法 2 的电流来进行较正!!!

mtk_imp_tracking() 对闭合电压补偿后，当作开路电压使用
通过对当前有负载的电池电压进行补偿，获得当前开路电压

BC1.1 充电协议，主要用来区分是插入的是 USB 还是充电器，如果是 USB 只能提供 500ma 充电，
如果是充电器，则可以大电流充电

```code
int hw_charging_get_charger_type(void)
{
#if 0
	return STANDARD_HOST;
	/* return STANDARD_CHARGER; //adaptor */
#else
    CHARGER_TYPE CHR_Type_num = CHARGER_UNKNOWN;

#ifdef CONFIG_MTK_USB2JTAG_SUPPORT
    if (usb2jtag_mode())
    {
        pr_err("[USB2JTAG] in usb2jtag mode, skip charger detection\n");
        return STANDARD_HOST;
    }
#endif

    /********* Step initial  ***************/
    hw_bc11_init();

    /********* Step DCD ***************/
    if (1 == hw_bc11_DCD())
    {
        /********* Step A1 ***************/
        if (1 == hw_bc11_stepA1())
        {
            CHR_Type_num = APPLE_2_1A_CHARGER;
            battery_log(BAT_LOG_CRTI, "step A1 : Apple 2.1A CHARGER!\r\n");
        }
        else
        {
            CHR_Type_num = NONSTANDARD_CHARGER;
            battery_log(BAT_LOG_CRTI, "step A1 : Non STANDARD CHARGER!\r\n");
        }
    }
    else
    {
        /********* Step A2 ***************/
        if (1 == hw_bc11_stepA2())
        {
            /********* Step B2 ***************/
            if (1 == hw_bc11_stepB2())
            {
                is_dcp_type = true;
                CHR_Type_num = STANDARD_CHARGER;
                battery_log(BAT_LOG_CRTI, "step B2 : STANDARD CHARGER!\r\n");
            }
            else
            {
                CHR_Type_num = CHARGING_HOST;
                battery_log(BAT_LOG_CRTI, "step B2 :  Charging Host!\r\n");
            }
        }
        else
        {
            CHR_Type_num = STANDARD_HOST;
            battery_log(BAT_LOG_CRTI, "step A2 : Standard USB Host!\r\n");
        }
    }

    /********* Finally setting *******************************/
    hw_bc11_done();

    return CHR_Type_num;
#endif
}
```

```code
概念: 
        ZCV：开路电压
        OCV: 开路电压
        VC：闭路电压
        CAR：库伦计
        DOD: 放电深度，100-DOD 即电容容量
        Cmax/Qmax: 电池容量
 
  
 初始化init------------pre_cc mode/CC Mode              topoff mode(CV mode)切换模式       Battery Full                         Battery hold
 1 初始化安全     1。做充电器的保护，                 1。做充电器的保护 2>，               1电池百分比总是显示100%             当充电器存在时，检查调用状态，
 充电器           2。如果(1)失败如果充电器电压为>6.5V 2。如果(1)失败如果充电器电压为>6.5V  注意:当充电,电池百分比仍然是100%。   1。如果呼叫活动，Vbat>4.05V停止充电注意:避免热的问题,
 2 检查V_bat电压   如果电池温度为>60c，则停止充电     如果电池温度为>60C，则停止充电
 是否大于3.4V决定 3.如果(2)通过，做充电算法:充电9秒，3.如果(2)通过，做充电算法。:充电10S
 进入CC模式，				停止充电1秒。
 检查UI SOC是否等于100
 决定进入电池满电状态
 ```


相关文件关系:

```code
Battery_common.c (s:\i841\mediatek\kernel\drivers\power) // 充电逻辑文件
Charging_hw_pmic.c (s:\i841\mediatek\platform\mt6582\kernel\drivers\power) // 具体的充电芯片，相关电池参数检测
Linear_charging.c (s:\i841\mediatek\kernel\drivers\power) // 充电状态控制，内部 PMIC
Switch_charging.c (s:\i841\mediatek\kernel\drivers\power) // 充电状态控制，外部 Charge IC
```

```c
//
// 【核心充电线程】
//kthread_run(bat_thread_kthread, NULL, "bat_thread_kthread");
// 设置定时器超时时间
int bat_thread_kthread(void *x)
{
    ktime_t ktime = ktime_set(3, 0); // 10s, 10* 1000 ms
    while (1)
    {
        // 这个是核心算法
        BAT_thread();
        ///
        // 0. 第一次执行时运行，获得开机显示电量，初始化电池算法 oam 参数
        // 开机时就会运行，只会运行一次，对电池算法 oam 方案进行初始化, 并获得开机显示电量百分比
        if (battery_meter_initilized == KAL_FALSE)
        {
            // 进行的一系列的电池参数与温度对应关系的表格的初始化，并根据电池当前电压，hw ocv 取一个较合适值，
            // 取合适值对应容量，再与 RTC 保存容量比较，选择一个合适量，为开机电池容量，最后初始化 oam 算法参数
            battery_meter_initial()
                MTK_MULTI_BAT_PROFILE_SUPPORT
                //电池曲线电池容量和在这里调用
                fgauge_get_profile_id();
            //开adc通道取电压值 时间单位是us
            IMM_GetOneChannelValue_Cali(BATTERY_ID_CHANNEL_NUM, &id_volt)
                //开adc通道读取通过adc通道把模拟信号转换成数字信号
                return IMM_auxadc_GetOneChannelValue_Cali(Channel, voltage)
                    IMM_auxadc_GetOneChannelValue(Channel, data, &rawvalue); /* HAL API */
            //使能一个时钟信号
            clk_prepare_enable(clk_auxadc);
            /* step1 check con2 if auxadc is busy step1如果auxadc忙，就检查con2*/
            while (AUXADC_DRV_ReadReg16((u16 *)AUXADC_CON2) & 0x01)
                mdelay(1);
            idle_count++;
            if (idle_count > 30)
            { //当大于30us时
                /* wait for idle time out/* wait for idle time out 等待空闲时间*/
                pr_err("[adc_api]: wait for auxadc idle time out\n");
                /* step2 clear bit 清除位*/
                if (adc_auto_set == 0)
                {
                    /* clear bit */
                    AUXADC_DRV_ClearBits16((u16 *)AUXADC_CON1, (1 << dwChannel));
                }
                /* step3  read channel and make sure old ready bit ==0 
/* step3读取通道，确保老的就绪位==0*/
                while (AUXADC_DRV_ReadReg16(AUXADC_DAT0 + dwChannel * 0x04) & (1 << 12))
                {
                    pr_debug("[adc_api]: wait for channel[%d] ready bit clear\n", dwChannel);
                    mdelay(1);
                    data_ready_count++;
                    if (data_ready_count > 30)
                    {
                        /* wait for idle time out等待空闲时间*/
                        pr_err("[adc_api]: wait for channel[%d] ready bit clear time out\n",
                               dwChannel);
                        /* step4 set bit  to trigger sample 
4 设置位触发样本 */
                        if (adc_auto_set == 0)
                            AUXADC_DRV_SetBits16((u16 *)AUXADC_CON1, (1 << dwChannel));

                        /* step5  read channel and make sure  ready bit ==1 
5 读取通道并确保准备位==1*/
                        udelay(25); /* we must dealay here for hw sample cahnnel data 我们必须在此处理cahnnel样本数据*/
                        while (0 == (AUXADC_DRV_ReadReg16(AUXADC_DAT0 + dwChannel * 0x04) & (1 << 12)))
                        {
                            pr_debug("[adc_api]: wait for channel[%d] ready bit ==1\n", dwChannel);
                            mdelay(1);
                            data_ready_count++;

                            if (data_ready_count > 30)
                            {
                                /* wait for idle time out 等待空闲时间*/
                                pr_err("[adc_api]: wait for channel[%d] data ready time out\n", dwChannel);
                                mutex_unlock(&mutex_get_cali_value);
                                return -3;
                            }
                        }
                        /* step6 read data 读取数据*/

                        channel[dwChannel] = AUXADC_DRV_ReadReg16(AUXADC_DAT0 + dwChannel * 0x04) & 0x0FFF;
                        //得到通过的数据
                        mt_auxadc_get_cali_data(channel[dwChannel], data, true);

                        SOC_BY_AUXADC
                        //电池曲线的表的数据调用
                        table_init();
                        //合电池曲线两个表的结构体，电池曲线 一个是百分比和电压 一个是电阻和电压
                        struct battery_profile_struct *profile_p;
                        struct r_profile_struct *profile_p_r_table;
                        //获取当前温度
                        int temperature = force_get_tbat(KAL_FALSE);

                        /* Re-constructure r-table profile according to current temperature */
                        //根据当前温度重新构造r-table剖面
                        profile_p_r_table = fgauge_get_profile_r_table(batt_meter_cust_data.temperature_t);
                        struct r_profile_struct *fgauge_get_profile_r_table(unsigned int temperature) switch (temperature)
                        {
                        case batt_meter_cust_data.temperature_t0:
                            return &r_profile_t0[g_fg_battery_id][0];
                        /*break;*/
                        case batt_meter_cust_data.temperature_t1:
                            return &r_profile_t1[g_fg_battery_id][0];
                        /*break;*/
                        case batt_meter_cust_data.temperature_t2:
                            return &r_profile_t2[g_fg_battery_id][0];
                        /*break;*/
                        case batt_meter_cust_data.temperature_t3:
                            return &r_profile_t3[g_fg_battery_id][0];
                        /*break;*/
                        case batt_meter_cust_data.temperature_t:
                            return &r_profile_temperature[0];
                        /*break;*/
                        default:
                            return NULL;
                            /*break;*/
                        }
                        if (profile_p_r_table == NULL)
                            bm_print(BM_LOG_CRTI,
                                     "[FGADC] fgauge_get_profile_r_table : create table fail !\r\n");
                        //根据当前温度重新构造r-table剖面
                        fgauge_construct_r_table_profile(temperature, profile_p_r_table);

                        SOC_BY_HW_FG
                        //电池容量的初始化
                        fgauge_initialization();
                        /* 1. HW initialization HW初始化 */
                        ret = battery_meter_ctrl(BATTERY_METER_CMD_HW_FG_INIT, NULL);
                        /* 2. SW algorithm initialization SW算法初始化 */
                        ret = battery_meter_ctrl(BATTERY_METER_CMD_GET_HW_OCV, &gFG_voltage); //电池控制函数ocv开路电压

                        ret = battery_meter_ctrl(BATTERY_METER_CMD_GET_HW_FG_CURRENT, &gFG_current); //电流

                        ret = battery_meter_ctrl(BATTERY_METER_CMD_GET_HW_FG_CAR, &gFG_columb);

                        gFG_temp = force_get_tbat(KAL_FALSE);   //获取温度
                        gFG_capacity = fgauge_read_capacity(0); //读取容量

                        gFG_capacity_by_c_init = gFG_capacity; //初始化容量
                        gFG_capacity_by_c = gFG_capacity;
                        gFG_capacity_by_v = gFG_capacity;

                        gFG_DOD0 = 100 - gFG_capacity; //放电深度的算法
                        bm_print(BM_LOG_CRTI, "[fgauge_initialization] gFG_DOD0 =%d %d\n", gFG_DOD0, gFG_capacity);

                        gFG_BATT_CAPACITY = fgauge_get_Q_max(gFG_temp); //获取最大容量

                        gFG_BATT_CAPACITY_init_high_current = fgauge_get_Q_max_high_current(gFG_temp); //初始化最大容量的高电流值
                        gFG_BATT_CAPACITY_aging = fgauge_get_Q_max(gFG_temp);                          //获取最大容量

                        ret = battery_meter_ctrl(BATTERY_METER_CMD_DUMP_REGISTER, NULL); //控制函数控制

                        bm_print(BM_LOG_CRTI,
                                 "[fgauge_initialization] Done HW_OCV:%d FG_Current:%d FG_CAR:%d tmp=%d capacity=%d Qmax=%d\n",
                                 gFG_voltage, gFG_current, gFG_columb, gFG_temp, gFG_capacity, gFG_BATT_CAPACITY);
                        //电池容量算法运行的初始化
                        fgauge_algo_run_init();
#ifdef INIT_SOC_BY_SW_SOC                                                       //如没插充电器或者处于关机状态 则停止为vbat测量充电
                        kal_bool charging_enable = KAL_FALSE;                   //没插充电器
#if defined(CONFIG_MTK_KERNEL_POWER_OFF_CHARGING) && !defined(SWCHR_POWER_PATH) //处于关机
                        if (get_boot_mode() != LOW_POWER_OFF_CHARGING_BOOT)     //
#endif
                            /*stop charging for vbat measurement 停止为vbat测量充电*/
                            battery_charging_control(CHARGING_CMD_ENABLE, &charging_enable);

                        msleep(50);
#endif
                        /* 1. Get Raw Data 得到原始数据*/
                        gFG_voltage = battery_meter_get_battery_voltage(KAL_TRUE);                            //获取原始数据的电压
                        gFG_voltage_init = gFG_voltage;                                                       //电压赋值
                        ret = battery_meter_ctrl(BATTERY_METER_CMD_GET_HW_FG_CURRENT, &gFG_current);          //电流控制
                        ret = battery_meter_ctrl(BATTERY_METER_CMD_GET_HW_FG_CURRENT_SIGN, &gFG_Is_Charging); //充电控制
                        //以下是电压的一些算法
                        gFG_voltage = gFG_voltage + fgauge_compensate_battery_voltage_recursion(gFG_voltage, 5); /* mV */
                        gFG_voltage = gFG_voltage + batt_meter_cust_data.ocv_board_compesate;

                        bm_print(BM_LOG_CRTI, "[FGADC] SWOCV : %d,%d,%d,%d,%d,%d\n",
                                 gFG_voltage_init, gFG_voltage, gFG_current, gFG_Is_Charging, gFG_resistance_bat,
                                 gFG_compensate_value); //初始化的电压，电压，充电电流 电阻  抵消值
#ifdef INIT_SOC_BY_SW_SOC
                        charging_enable = KAL_TRUE;
                        battery_charging_control(CHARGING_CMD_ENABLE, &charging_enable); //如果有插充电器，开始充电
#endif
                        ret = battery_meter_ctrl(BATTERY_METER_CMD_GET_HW_FG_CAR, &gFG_columb);

                        /* 1.1 Average FG_voltage FG的平均电压*/
                        for (i = 0; i < batt_meter_cust_data.fg_vbat_average_size; i++)
                            FGvbatVoltageBuffer[i] = gFG_voltage;

                        FGbatteryVoltageSum = gFG_voltage * batt_meter_cust_data.fg_vbat_average_size;
                        gFG_voltage_AVG = gFG_voltage;

#ifdef Q_MAX_BY_CURRENT
                        /* 1.2 Average FG_currentFG平均电流 */
                        for (i = 0; i < FG_CURRENT_AVERAGE_SIZE; i++)
                            FGCurrentBuffer[i] = gFG_current;

                        FGCurrentSum = gFG_current * FG_CURRENT_AVERAGE_SIZE;
                        gFG_current_AVG = gFG_current;
#endif

                        /* 2. Calculate battery capacity by VBAT 用VBAT计算电池容量*/
                        gFG_capacity_by_v = fgauge_read_capacity_by_v(gFG_voltage);
                        gFG_capacity_by_v_init = gFG_capacity_by_v;

                        /* 3. Calculate battery capacity by Coulomb Counter 用库仑计数器计算电池容量 */
                        gFG_capacity_by_c = fgauge_read_capacity(1);

                        /* 4. update DOD0 更新DOD0 */

                        dod_init();

                        gFG_current_auto_detect_R_fg_count = 0;

                        for (i = 0; i < 10; i++)
                        {
                            ret = battery_meter_ctrl(BATTERY_METER_CMD_GET_HW_FG_CURRENT, &gFG_current);

                            gFG_current_auto_detect_R_fg_total += gFG_current;
                            gFG_current_auto_detect_R_fg_count++;
                        }

                        /* double check 再次确认*/
                        if (gFG_current_auto_detect_R_fg_total <= 0)
                        {
                            bm_print(BM_LOG_CRTI, "gFG_current_auto_detect_R_fg_total=0, need double check\n");

                            gFG_current_auto_detect_R_fg_count = 0;

                            for (i = 0; i < 10; i++)
                            {
                                ret = battery_meter_ctrl(BATTERY_METER_CMD_GET_HW_FG_CURRENT, &gFG_current);

                                gFG_current_auto_detect_R_fg_total += gFG_current;
                                gFG_current_auto_detect_R_fg_count++;
                            }
                        }
                        //自动检测电池曲线表中的电阻和电压 结果值
                        gFG_current_auto_detect_R_fg_result =
                            gFG_current_auto_detect_R_fg_total / gFG_current_auto_detect_R_fg_count;
#if !defined(DISABLE_RFG_EXIST_CHECK)
                        if (gFG_current_auto_detect_R_fg_result <= batt_meter_cust_data.current_detect_r_fg)
                        {
                            g_auxadc_solution = 1;

                            bm_print(BM_LOG_CRTI,
                                     "[FGADC] Detect NO Rfg, use AUXADC report. (%d=%d/%d)(%d)\r\n",
                                     gFG_current_auto_detect_R_fg_result, gFG_current_auto_detect_R_fg_total,
                                     gFG_current_auto_detect_R_fg_count, g_auxadc_solution);
                        }
                        else
                        {
                            if (g_auxadc_solution == 0)
                            {
                                g_auxadc_solution = 0;

                                bm_print(BM_LOG_CRTI,
                                         "[FGADC] Detect Rfg, use FG report. (%d=%d/%d)(%d)\r\n",
                                         gFG_current_auto_detect_R_fg_result,
                                         gFG_current_auto_detect_R_fg_total,
                                         gFG_current_auto_detect_R_fg_count, g_auxadc_solution);
                            }
                            else
                            {
                                bm_print(BM_LOG_CRTI,
                                         "[FGADC] Detect Rfg, but use AUXADC report. due to g_auxadc_solution=%d \r\n",
                                         g_auxadc_solution);
                            }
                        }
#endif
                        /* 5. Logging  记录*/
                        bm_print(BM_LOG_CRTI,
                                 "[FGADC] %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
                                 gFG_Is_Charging, gFG_current, gFG_columb, gFG_voltage, gFG_capacity_by_v,
                                 gFG_capacity_by_c, gFG_capacity_by_c_init, gFG_BATT_CAPACITY,
                                 gFG_BATT_CAPACITY_aging, gFG_compensate_value, gFG_ori_voltage,
                                 batt_meter_cust_data.ocv_board_compesate, batt_meter_cust_data.r_fg_board_slope,
                                 gFG_voltage_init, batt_meter_cust_data.minerroroffset, gFG_DOD0, gFG_DOD1,
                                 batt_meter_cust_data.car_tune_value, batt_meter_cust_data.aging_tuning_value);
                        update_fg_dbg_tool_value();
                        g_fg_dbg_bat_volt = gFG_voltage_init;

                        if (gFG_Is_Charging == KAL_TRUE)
                            g_fg_dbg_bat_current = 1 - gFG_current - 1;
                        else
                            g_fg_dbg_bat_current = gFG_current;

                        g_fg_dbg_bat_zcv = gFG_voltage;
                        g_fg_dbg_bat_temp = gFG_temp;
                        g_fg_dbg_bat_r = gFG_resistance_bat;
                        g_fg_dbg_bat_car = gFG_columb;
                        g_fg_dbg_bat_qmax = gFG_BATT_CAPACITY_aging;
                        g_fg_dbg_d0 = gFG_DOD0;
                        g_fg_dbg_d1 = gFG_DOD1;
                        g_fg_dbg_percentage = bat_get_ui_percentage();
                        g_fg_dbg_percentage_fg = gFG_capacity_by_c;
                        g_fg_dbg_percentage_voltmode = gfg_percent_check_point;

                        SOC_BY_SW_FG
                        table_init(); //电池曲线的表的数据调用
                        oam_init();   //oam算法的初始化
                        int ret = 0;
                        signed int vbat_capacity = 0;
                        kal_bool charging_enable = KAL_FALSE;

                        /*stop charging for vbat measurement 停止为vbat测量充电*/
                        battery_charging_control(CHARGING_CMD_ENABLE, &charging_enable);

                        msleep(50);

                        g_booting_vbat = 5; /* set avg times 设置avg时间*/
                        ret = battery_meter_ctrl(BATTERY_METER_CMD_GET_HW_OCV, &gFG_voltage);
                        ret = battery_meter_ctrl(BATTERY_METER_CMD_GET_ADC_V_BAT_SENSE, &g_booting_vbat);

                        gFG_capacity_by_v = fgauge_read_capacity_by_v(gFG_voltage);
                        vbat_capacity = fgauge_read_capacity_by_v(g_booting_vbat);

                        if (bat_is_charger_exist() == KAL_TRUE)
                        {
                            bm_print(BM_LOG_CRTI, "[oam_init_inf] gFG_capacity_by_v=%d, vbat_capacity=%d,\n",
                                     gFG_capacity_by_v, vbat_capacity);

                            /* to avoid plug in cable without battery, then plug in battery to make hw soc = 100% */
                            //为了避免没有电池的情况下插入电缆，然后插入电池使hw soc = 100%
                            /* if the difference bwtween ZCV and vbat is too large, using vbat instead ZCV */
                            //如果ZCV和vbat之间的差异太大，用vbat代替ZCV
                            if (((gFG_capacity_by_v == 100) && (vbat_capacity < batt_meter_cust_data.cust_poweron_max_vbat_tolrance)) 
                            || (abs(gFG_capacity_by_v - vbat_capacity) > batt_meter_cust_data.cust_poweron_delta_vbat_tolrance))
                            {
                                bm_print(BM_LOG_CRTI,
                                         "[oam_init] fg_vbat=(%d), vbat=(%d), set fg_vat as vat\n",
                                         gFG_voltage, g_booting_vbat);

                                gFG_voltage = g_booting_vbat;
                                gFG_capacity_by_v = vbat_capacity;
                            }
                        }

                        gFG_capacity_by_v_init = gFG_capacity_by_v;

                        dod_init();

                        gFG_BATT_CAPACITY_aging = fgauge_get_Q_max(force_get_tbat(KAL_FALSE));

                        /* oam_v_ocv_1 = gFG_voltage; */
                        /* oam_v_ocv_2 = gFG_voltage; */

                        oam_v_ocv_init = fgauge_read_v_by_d(gFG_DOD0);
                        oam_v_ocv_2 = oam_v_ocv_1 = oam_v_ocv_init;
                        g_vol_bat_hw_ocv = gFG_voltage;

                        /* vbat = 5; //set avg times */
                        /* ret = battery_meter_ctrl(BATTERY_METER_CMD_GET_ADC_V_BAT_SENSE, &vbat); */
                        /* oam_r_1 = fgauge_read_r_bat_by_v(vbat); */
                        oam_r_1 = fgauge_read_r_bat_by_v(gFG_voltage);
                        oam_r_2 = oam_r_1;

                        oam_d0 = gFG_DOD0;
                        oam_d_5 = oam_d0;
                        oam_i_ori = gFG_current;
                        g_d_hw_ocv = oam_d0;

                        if (oam_init_i == 0)
                        {
                            bm_print(BM_LOG_CRTI,
                                     "[oam_init] oam_v_ocv_1,oam_v_ocv_2,oam_r_1,oam_r_2,oam_d0,oam_i_ori\n");
                            oam_init_i = 1;
                        }

                        bm_print(BM_LOG_CRTI, "[oam_init] %d,%d,%d,%d,%d,%d\n",
                                 oam_v_ocv_1, oam_v_ocv_2, oam_r_1, oam_r_2, oam_d0, oam_i_ori);

                        bm_print(BM_LOG_CRTI, "[oam_init_inf] hw_OCV, hw_D0, RTC, D0, oam_OCV_init, tbat\n");
                        bm_print(BM_LOG_CRTI,
                                 "[oam_run_inf] oam_OCV1, oam_OCV2, vbat, I1, I2, R1, R2, Car1, Car2,qmax, tbat\n");
                        bm_print(BM_LOG_CRTI, "[oam_result_inf] D1, D2, D3, D4, D5, UI_SOC\n");

                        bm_print(BM_LOG_CRTI, "[oam_init_inf] %d, %d, %d, %d, %d, %d\n",
                                 gFG_voltage, (100 - fgauge_read_capacity_by_v(gFG_voltage)), g_rtc_fg_soc,
                                 gFG_DOD0, oam_v_ocv_init, force_get_tbat(KAL_FALSE));

                        charging_enable = KAL_TRUE;
                        battery_charging_control(CHARGING_CMD_ENABLE, &charging_enable)
                    }

                    // 1. 判断是插入的是否充电器还是电脑 USB，看能不能进行充电
                    // 如果连接的 USB 线为 USB 充电线，或者电脑 USB 线，则打开 USB，
                    // 这里会通过  BC1.1 来判断是电脑 USB 还是 USB 充电器，来决定充电电流
                    // 否则连接的不是充电线或者 USB 作为一个从设备使用，要断开 USB?
                    mt_battery_charger_detect_check();
                    // 1. 判断是插入的是否充电器还是电脑 USB，看能不能进行充电
                    // 如果连接的 USB 线为 USB 充电线，或者电脑 USB 线，则打开 USB，
                    // 这里会通过  BC1.1 来判断是电脑 USB 还是 USB 充电器，来决定充电电流
                    // 否则连接的不是充电线或者 USB 作为一个从设备使用，要断开 USB?

#ifdef CONFIG_MTK_BQ25896_SUPPORT
                    /*New low power feature of MT6531: disable charger CLK without CHARIN.
* MT6351 API abstracted in charging_hw_bw25896.c. Any charger with MT6351 needs to set this.
* Compile option is not limited to CONFIG_MTK_BQ25896_SUPPORT.
* PowerDown = 0
新的低功耗功能的MT6531:禁用充电器CLK没有CHARIN。
* MT6351 API抽象在charging_hw_bw25896.c。任何MT6351的充电器都需要设置这个。
编译选项不限于CONFIG_MTK_BQ25896_SUPPORT。
* PowerDown = 0
*/
                    unsigned int pwr;
#endif
                    if (upmu_is_chr_det() == KAL_TRUE)
                    {
                        wake_lock(&battery_suspend_lock);

#if !defined(CONFIG_MTK_DUAL_INPUT_CHARGER_SUPPORT)
                        //判断充电器是否存在
                        BMT_status.charger_exist = KAL_TRUE;
#endif

#if defined(CONFIG_MTK_WIRELESS_CHARGER_SUPPORT)
                        //充电类型的检测
                        mt_charger_type_detection();

                        if ((BMT_status.charger_type == STANDARD_HOST) || (BMT_status.charger_type == CHARGING_HOST))
                        {
                            //打开 USB
                            mt_usb_connect();
                        }
#else
#if !defined(CONFIG_MTK_DUAL_INPUT_CHARGER_SUPPORT)
                        if (BMT_status.charger_type == CHARGER_UNKNOWN)
                        {
#else
                        if ((BMT_status.charger_type == CHARGER_UNKNOWN) &&
                            (DISO_data.diso_state.cur_vusb_state == DISO_ONLINE))
                        {
#endif
                            mt_charger_type_detection();

                            if ((BMT_status.charger_type == STANDARD_HOST) || (BMT_status.charger_type == CHARGING_HOST))
                            {
                                mt_usb_connect();
                            }
                        }
#endif

#ifdef CONFIG_MTK_BQ25896_SUPPORT
                        /*New low power feature of MT6531: disable charger CLK without CHARIN.
* MT6351 API abstracted in charging_hw_bw25896.c. Any charger with MT6351 needs to set this.
* Compile option is not limited to CONFIG_MTK_BQ25896_SUPPORT.
* PowerDown = 0
新的低功耗功能的MT6531:禁用充电器CLK没有CHARIN。
* MT6351 API抽象在charging_hw_bw25896.c。任何MT6351的充电器都需要设置这个。
编译选项不限于CONFIG_MTK_BQ25896_SUPPORT。
* PowerDown = 0
*   
*/
                        pwr = 0;
                        battery_charging_control(CHARGING_CMD_SET_CHRIND_CK_PDN, &pwr);
#endif

                        battery_log(BAT_LOG_FULL, "[BAT_thread]Cable in, CHR_Type_num=%d\r\n",
                                    BMT_status.charger_type);
                    }
                    else
                    {
                        wake_unlock(&battery_suspend_lock);

                        BMT_status.charger_exist = KAL_FALSE;
                        BMT_status.charger_type = CHARGER_UNKNOWN;
                        BMT_status.bat_full = KAL_FALSE;
                        BMT_status.bat_in_recharging_state = KAL_FALSE;
                        BMT_status.bat_charging_state = CHR_PRE;
                        BMT_status.total_charging_time = 0;
                        BMT_status.PRE_charging_time = 0;
                        BMT_status.CC_charging_time = 0;
                        BMT_status.TOPOFF_charging_time = 0;
                        BMT_status.POSTFULL_charging_time = 0;

                        battery_log(BAT_LOG_CRTI, "[BAT_thread]Cable out \r\n");

                        mt_usb_disconnect();

#ifdef CONFIG_MTK_BQ25896_SUPPORT
                        /*New low power feature of MT6531: disable charger CLK without CHARIN.
* MT6351 API abstracted in charging_hw_bw25896.c. Any charger with MT6351 needs to set this.
* Compile option is not limited to CONFIG_MTK_BQ25896_SUPPORT.
* PowerDown = 1
新的低功耗功能的MT6531:禁用充电器CLK没有CHARIN。
* MT6351 API抽象在charging_hw_bw25896.c。任何MT6351的充电器都需要设置这个。
编译选项不限于CONFIG_MTK_BQ25896_SUPPORT。
*关机= 1
*/
                        pwr = 1;
                        battery_charging_control(CHARGING_CMD_SET_CHRIND_CK_PDN, &pwr);
#endif

                        ///
                        // 2. 通过具体的充电芯片来获得电池信息，充电信息, 获得电池电量百分比
                        // 通过 oam 算法，获得电量百分比
                        mt_battery_GetBatteryData();
                        //获得电池信息
                        bat_vol = battery_meter_get_battery_voltage(KAL_TRUE);
                        Vsense = battery_meter_get_VSense(); //来限制输出电流
                        if (upmu_is_chr_det() == KAL_TRUE)
                            ICharging = battery_meter_get_charging_current(); //获取当前的充电电流
                        else
                            ICharging = 0;

                        //获得充电信息
                        charger_vol = battery_meter_get_charger_voltage();     //获取充电电压
                        temperature = battery_meter_get_battery_temperature(); //获取充电温度
                        temperatureV = battery_meter_get_tempV();              //获取充电温度V
                        temperatureR = battery_meter_get_tempR(temperatureV);  //获取充电温度R

                        if (bat_meter_timeout == KAL_TRUE || bat_spm_timeout == TRUE || fg_wake_up_bat == KAL_TRUE)
                        {
                            // 获得电池电量百分比
                            SOC = battery_meter_get_battery_percentage();
                            /* if (bat_spm_timeout == true) */
                            /* BMT_status.UI_SOC = battery_meter_get_battery_percentage(); */

                            bat_meter_timeout = KAL_FALSE;
                            bat_spm_timeout = FALSE;
                        }
                        else
                        {
                            if (previous_SOC == -1)
                                SOC = battery_meter_get_battery_percentage();
                            else
                                SOC = previous_SOC;
                        }

                        ZCV = battery_meter_get_battery_zcv(); //获取开路电压

                        BMT_status.ICharging =
                            mt_battery_average_method(BATTERY_AVG_CURRENT, &batteryCurrentBuffer[0], ICharging,
                                                      &icharging_sum, batteryIndex); //求电流的平均算法

                        if (previous_SOC == -1 && bat_vol <= batt_cust_data.v_0percent_tracking)
                        {
                            battery_log(BAT_LOG_CRTI,
                                        "battery voltage too low, use ZCV to init average data.\n");
                            BMT_status.bat_vol =
                                mt_battery_average_method(BATTERY_AVG_VOLT, &batteryVoltageBuffer[0], ZCV,
                                                          &bat_sum, batteryIndex); //求电压的平均算法
                        }
                        else
                        {
                            BMT_status.bat_vol =
                                mt_battery_average_method(BATTERY_AVG_VOLT, &batteryVoltageBuffer[0], bat_vol,
                                                          &bat_sum, batteryIndex);
                        }

                        if (battery_cmd_thermal_test_mode == 1)
                        {
                            battery_log(BAT_LOG_CRTI, "test mode , battery temperature is fixed.\n"); //测试模式，电池温度是固定的。
                        }
                        else
                        {
                            BMT_status.temperature =
                                mt_battery_average_method(BATTERY_AVG_TEMP, &batteryTempBuffer[0], temperature,
                                                          &temperature_sum, batteryIndex);
                        }

                        //电池状态的初始化
                        BMT_status.Vsense = Vsense;
                        BMT_status.charger_vol = charger_vol;
                        BMT_status.temperatureV = temperatureV;
                        BMT_status.temperatureR = temperatureR;
                        BMT_status.SOC = SOC;
                        BMT_status.ZCV = ZCV;
                        BMT_status.CURRENT_NOW = BMT_status.IBattery;

                        if (BMT_status.charger_exist == KAL_FALSE)
                        {
                            signed int soc_tmp = BMT_status.SOC;

                            if (soc_tmp > previous_SOC && previous_SOC >= 0)
                                BMT_status.SOC = previous_SOC;
                        }

                        previous_SOC = BMT_status.SOC;

                        batteryIndex++;
                        if (batteryIndex >= BATTERY_AVERAGE_SIZE)
                            batteryIndex = 0;

                        if (g_battery_soc_ready == KAL_FALSE)
                            g_battery_soc_ready = KAL_TRUE;

                        battery_log(BAT_LOG_CRTI,
                                    "AvgVbat=(%d,%d),AvgI=(%d,%d),VChr=%d,AvgT=(%d,%d),SOC=(%d,%d),UI_SOC=%d,ZCV=%d bcct:%d:%d I:%d\n",
                                    BMT_status.bat_vol, bat_vol, BMT_status.ICharging, ICharging,
                                    BMT_status.charger_vol, BMT_status.temperature, temperature,
                                    previous_SOC, BMT_status.SOC, BMT_status.UI_SOC, BMT_status.ZCV,
                                    g_bcct_flag, get_usb_current_unlimited(), get_bat_charging_current_level());

                        // 3. 电池温度保护
                        // 电池温度检查，如果温度超过 60 度，关机重启
                        mt_battery_thermal_check();
                        BMT_status.temperature = battery_cmd_thermal_test_mode_value; //获取当前电池温度
                        if (BMT_status.temperature >= 60)
                        { // 电池温度检查，如果温度超过 60 度，关机重启
#if defined(CONFIG_MTK_JEITA_STANDARD_SUPPORT)
/* ignore default rule 忽略默认规则*/
#else
                            if (BMT_status.temperature >= 60)
                            { // 电池温度检查，如果温度超过 60 度，关机重启
#if defined(CONFIG_POWER_EXT)
                                battery_log(BAT_LOG_CRTI,
                                            "[BATTERY] CONFIG_POWER_EXT, no update battery update power down.\n");
#else
                                {
                                    if ((g_platform_boot_mode == META_BOOT) || (g_platform_boot_mode == ADVMETA_BOOT) 
                                    || (g_platform_boot_mode == ATE_FACTORY_BOOT))
                                    { //绕过温度检查
                                        battery_log(BAT_LOG_FULL,
                                                    "[BATTERY] boot mode = %d, bypass temperature check\n",
                                                    g_platform_boot_mode);
                                    }
                                    else
                                    {
                                        struct battery_data *bat_data = &battery_main;
                                        struct power_supply *bat_psy = bat_data->psy;

                                        battery_log(BAT_LOG_CRTI,
                                                    "[Battery] Tbat(%d)>=60, system need power down.\n",
                                                    BMT_status.temperature);

                                        bat_data->BAT_CAPACITY = 0;

                                        power_supply_changed(bat_psy);

                                        if (BMT_status.charger_exist == KAL_TRUE)
                                        {
/* can not power down due to charger exist, so need reset system
忽略由于充电器的存在而导致的断电，因此需要复位 */
                                            orderly_reboot();
                                        }
                                        /* avoid SW no feedback 避免软件无反馈*/
                                        orderly_poweroff(true);

                                        /
                                            // 4. 电池状态检查
                                            //  对电池状态进行检查，如果有问题，则会调用 printk() 进行打印
                                            mt_battery_notify_check();
                                        g_BatteryNotifyCode = 0x0000;

                                        if (g_BN_TestMode == 0x0000)
                                        { /* for normal case 对于正常情况*/
                                            battery_log(BAT_LOG_FULL, "[BATTERY] mt_battery_notify_check\n");
                                            mt_battery_notify_VCharger_check();
                                            mt_battery_notify_VBatTemp_check();
                                            mt_battery_notify_ICharging_check();
                                            mt_battery_notify_VBat_check();
                                            mt_battery_notify_TotalChargingTime_check();
                                        }
                                        else
                                        { /* for UI test */
                                            //  对电池状态进行检查，如果有问题，则会调用 printk() 进行打印
                                            mt_battery_notify_UI_test();
                                        }

                                        //
                                        // 5. 调用具本的硬件相关函数进行充电，充电时会进行 CC/CV 之类的状态机切换就是在这里进行的
                                        // 如果存在充电线，则调用具体充电芯片相关的函数进行充电
                                        if (BMT_status.charger_exist == KAL_TRUE)
                                        {
                                            // 检查电池状态，设置到 BMT_status.bat_charging_state 中
                                            mt_battery_CheckBatteryStatus();
                                            // 检查电池状态，设置到 BMT_status.bat_charging_state 中
                                            static void mt_battery_CheckBatteryStatus(void)
                                            {
                                                battery_log(BAT_LOG_FULL, "[mt_battery_CheckBatteryStatus] cmd_discharging=(%d)\n",
                                                            cmd_discharging);
                                                if (cmd_discharging == 1)
                                                { //检查到没有插充电器
                                                    battery_log(BAT_LOG_CRTI,
                                                                "[mt_battery_CheckBatteryStatus] cmd_discharging=(%d)\n",
                                                                cmd_discharging);
                                                    BMT_status.bat_charging_state = CHR_ERROR;
                                                    battery_charging_control(CHARGING_CMD_SET_ERROR_STATE, &cmd_discharging);
                                                    return;
                                                }
                                                else if (cmd_discharging == 0)
                                                { //检查到有插充电器
                                                    BMT_status.bat_charging_state = CHR_PRE;
                                                    battery_charging_control(CHARGING_CMD_SET_ERROR_STATE, &cmd_discharging);
                                                    cmd_discharging = -1;
                                                }
                                                if (mt_battery_CheckBatteryTemp() != PMU_STATUS_OK)
                                                { //检查电池温度
                                                    BMT_status.bat_charging_state = CHR_ERROR;
                                                    return;
                                                }

                                                if (mt_battery_CheckChargerVoltage() != PMU_STATUS_OK)
                                                { //检查电压
                                                    BMT_status.bat_charging_state = CHR_ERROR;
                                                    return;
                                                }
#if defined(STOP_CHARGING_IN_TAKLING)
                                                if (mt_battery_CheckCallState() != PMU_STATUS_OK)
                                                { //检查调用状态
                                                    BMT_status.bat_charging_state = CHR_HOLD;
                                                    return;
                                                }
#endif
                                                if (mt_battery_CheckChargingTime() != PMU_STATUS_OK)
                                                { //检查充电时间
                                                    BMT_status.bat_charging_state = CHR_ERROR;
                                                    return;
                                                }

                                                // 充电策略，这里有两个文件: switch_charging.c 和 linear_charging.c
                                                // 他们的关系是，如果定义了任一外部充电 IC，则选择 switch_charging.c 的函数，否则就是 linear_charging.c 的函数
                                                // 这里就是调用具体的芯片的充电相关函数进行充电
                                                mt_battery_charging_algorithm(); //如果定义了任一外部充电 IC，则选择 switch_charging.c 的函数，否则就是 linear_charging.c 的函数
                                                void mt_battery_charging_algorithm()
                                                {
                                                    battery_charging_control(CHARGING_CMD_RESET_WATCH_DOG_TIMER, NULL);

                                                    /* Generate AICR upper bound by AICL 用AICL生成AICR上界 */
                                                    if (!mtk_is_pep_series_connect())
                                                    {
                                                        battery_charging_control(CHARGING_CMD_RUN_AICL,
                                                                                 &g_aicr_upper_bound);
                                                    }

                                                    mtk_pep20_check_charger();
                                                    mtk_pep_check_charger();
                                                    switch (BMT_status.bat_charging_state)
                                                    {
                                                    case CHR_PRE: //pre模式
                                                        /*1。Init安全定时器。2 检查VBAT是否进入cc模式，检查USOC是否进入电池全态*/
                                                        BAT_PreChargeModeAction();
                                                        break;

                                                    case CHR_CC: //恒流模式作用
                                                        /*1。做充电器的保护2。(1)失败>如果充电器电压>6.5V 2>如果电池温度>60c停止充电。
                                                        如果(2)通过，做充电算法。:充电9秒，停止充电1秒。*/
                                                        BAT_ConstantCurrentModeAction();
                                                        break;

                                                    case CHR_TOP_OFF: //顶部模式(CV模式)。
                                                        //1。做充电器的保护2。如果(1)失败如果充电器电压>6.5V
                                                        //2>如果电池温度>500停止充电3.
                                                        //	如果(2)通过，做充电算法。:充电10年代
                                                        BAT_TopOffModeAction();
                                                        break;

                                                    case CHR_BATFULL: //满电模式
                                                        /*1。电池百分比总是显示100%注意:当充电,电池百分比仍然是100%。*/
                                                        BAT_BatteryFullAction();
                                                        break;

                                                    case CHR_HOLD: //保持模式
                                                        /*当充电器存在时，检查呼叫状态1。如果呼叫激活，Vbat>4.05V停止充电注意:避免热问题和燃料计量问题*/
                                                        BAT_BatteryHoldAction();
                                                        break;

                                                    case CHR_ERROR: //错误状态
                                                        //禁用充电器  禁用PE + / PE + 20
                                                        BAT_BatteryStatusFailAction();
                                                        break;
                                                    }

                                                    battery_charging_control(CHARGING_CMD_DUMP_REGISTER, NULL);
                                                }
                                            }
                                            ///
                                            // 6. 更新电池显示状态
                                            // 更新设置节点的内容:
                                            //  /sys/class/power_supply/下的文件夹
                                            //  wireless_main
                                            //  battery_main
                                            //  ac_main
                                            //  usb_main
                                            mt_battery_update_status();
#if defined(CONFIG_POWER_EXT)
                                            battery_log(BAT_LOG_CRTI, "[BATTERY] CONFIG_POWER_EXT, no update Android.\n");
#else
                                            {
                                                if (skip_battery_update == KAL_FALSE)
                                                {
                                                    battery_log(BAT_LOG_FULL, "mt_battery_update_status.\n");
                                                    usb_update(&usb_main);
                                                    ac_update(&ac_main);
                                                    wireless_update(&wireless_main);
                                                    battery_update(&battery_main);
                                                }
                                                else
                                                {
                                                    battery_log(BAT_LOG_CRTI, "skip mt_battery_update_status.\n");
                                                    skip_battery_update = KAL_FALSE;
                                                }
                                            }

#endif
                                            // 睡眠等待唤醒
                                            wait_event(bat_thread_wq, (bat_thread_timeout == KAL_TRUE));
                                            // 每 10s 启动一次
                                            hrtimer_start(&battery_kthread_timer, ktime, HRTIMER_MODE_REL);
                                            ktime = ktime_set(BAT_TASK_PERIOD, 0); // 10s, 10* 1000 ms
                                        }
                                        /
                                            // 电池过充保护相关检测与初始化，他 2s 检测一次
                                            charger_hv_detect_sw_workaround_init();
                                        charger_hv_detect_thread = kthread_run(charger_hv_detect_sw_thread_handler, 0, "mtk charger_hv_detect_sw_workaround");
                                        // 这个函数是周期来检查电池电压是否超出限制，即过充保护，超出了就不能充电了
                                        charger_hv_detect_sw_thread_handler() do
                                        {
                                            ktime = ktime_set(0, BAT_MS_TO_NS(2000));
                                            if (chargin_hw_init_done)
                                                // 高压检测，应该是电池超过这个电压时，就不可以充电了
                                                battery_charging_control(CHARGING_CMD_SET_HV_THRESHOLD, &hv_voltage);
                                            // 对应 PMIC charging_set_hv_threshold()
                                            wait_event_interruptible(charger_hv_detect_waiter, (charger_hv_detect_flag == KAL_TRUE));
                                            // 如果检测到充电器，则检测下电池是否存在 检测电池是否存在,通过读取 PMIC 的 CHR_CON7
                                            if ((upmu_is_chr_det() == KAL_TRUE))
                                            {
                                                // 检测电池是否存在
                                                check_battery_exist();
                                            }
                                            hrtimer_start(&charger_hv_detect_timer, ktime, HRTIMER_MODE_REL);
                                        }
                                        while (!kthread_should_stop())
                                            ;

                                        //
                                        // battery notofy UI 电池通知上层
                                        platform_device_register(&MT_batteryNotify_device)
                                            platform_driver_register(&mt_batteryNotify_driver)
                                                mt_batteryNotify_probe()
                                                    ret_device_file = device_create_file(&(dev->dev), &dev_attr_BatteryNotify);
                                        ret_device_file = device_create_file(&(dev->dev), &dev_attr_BN_TestMode);
                                        battery_dir = proc_mkdir("mtk_battery_cmd", NULL);


```


电池测量模块初始化:
```code
module_init(battery_meter_init);
battery_meter_init(void)
platform_device_register(&battery_meter_device);
platform_driver_register(&battery_meter_driver);
// 调用对应的 probe 函数
battery_meter_probe()
// 测量模块接口设置【核心在于 bm_ctrl_cmd 函数】
battery_meter_ctrl = bm_ctrl_cmd;

*signed int bm_ctrl_cmd(enum BATTERY_METER_CTRL_CMD cmd, void data) { signed int status = 0;
static signed int init = -1;
if (init == -1) {
init = 0;
bm_func[BATTERY_METER_CMD_HW_FG_INIT] = fgauge_initialization;
bm_func[BATTERY_METER_CMD_GET_HW_FG_CURRENT] = fgauge_read_current;
bm_func[BATTERY_METER_CMD_GET_HW_FG_CURRENT_SIGN] = fgauge_read_current_sign;
bm_func[BATTERY_METER_CMD_GET_HW_FG_CAR] = fgauge_read_columb;
bm_func[BATTERY_METER_CMD_HW_RESET] = fgauge_hw_reset;
bm_func[BATTERY_METER_CMD_GET_ADC_V_BAT_SENSE] = read_adc_v_bat_sense;
bm_func[BATTERY_METER_CMD_GET_ADC_V_I_SENSE] = read_adc_v_i_sense;
bm_func[BATTERY_METER_CMD_GET_ADC_V_BAT_TEMP] = read_adc_v_bat_temp;
bm_func[BATTERY_METER_CMD_GET_ADC_V_CHARGER] = read_adc_v_charger;
bm_func[BATTERY_METER_CMD_GET_HW_OCV] = read_hw_ocv;
bm_func[BATTERY_METER_CMD_DUMP_REGISTER] = dump_register_fgadc;
}
if (cmd < BATTERY_METER_CMD_NUMBER) {
if (bm_func[cmd] != NULL)
status = bm_func[cmd] (data);
} else
return STATUS_UNSUPPORTED;
return status;
}
```
