# GPIO调试技巧

## 打开内核GPIO_GPIO_SYSFS控制可以直接使用adb来控制GPIO

```bash
导出            /sys/class/gpio/ # echo 12 > export
设置方向        /sys/class/gpio/ # echo out > gpio12/direction
查看方向        /sys/class/gpio/ # cat gpio12/direction
设置输出        /sys/class/gpio/ # echo 1 > gpio12/value
查看输出值      /sys/class/gpio/ # cat gpio12/value
取消导出        /sys/class/gpio/ # echo 12 > unexport
```
