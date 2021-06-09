# ATA GPS配置及开发

## 开发背景

## 客制化内容

S600 6739平台GPS测试一直失败，抓取log分析，注释掉GPS_Open()函数里面的

```code
#if 0//defined(MTK_GPS_MT3332) //wangfeng del
    err = mnl_write_attr("/sys/class/gpsdrv/gps/pwrctl", 4);
    if (err != 0) {
        LOGD(TAG"GPS_Open: GPS power-on error: %d\n", err);
        return (-1);
    }
#endif
```

## 关健代码相关修改

## 代码移植包供参考

## 总结
