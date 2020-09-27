# Memory存储

参考资料：
[eMMC/eMCP将成为移动设备嵌入式存储主导技术](http://blog.fang.com/39645797/16299159/articledetail.htm)

## 调试方法

- 查询项目主板BOM对应的memory型号，去MTK网站下载QVL时序文件包，注意要对应好CPU平台及memory的类型【 EMCP、LPDDR2等 】
下载回来的时序将时序文件客制化到preloader的excel表中，注意分区也需要客制化
- 如果MTK网站的QVL中没有查到memory的型号，说明mtk还未认证此memory，可以尝试如下方案解决：
  - 咨询供应商此memory和别的哪颗memory时序、类型相似，到mtk网站下载相似的memory时序
  - 按照mempry容量大小查询mtk默认excel表中其他芯片的时序文件进行对比分析，使用一个最接近的时序，分区也是一样的方式处理
  - 时序文件中分为emmc时序和ddr时序，emmc id可以可以查找对应id使用对应的时序，ddr使用相似的ddr时序
