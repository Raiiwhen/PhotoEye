# 光电眼

为了避免晚上玩手机被发现的接近报警器。

by Raii  

eMail masterraii@icloud.com

---

* 2020-05-18 完成硬件
  * SPI接口的1.14寸LCD显示屏
  * VL53L0x激光测距
  * 蜂鸣器
  * 作为光赛小车避障的SPI从机接口
  * 多层PCB容纳Φ12十字激光
  * 硬件拓展：作为光电传感器平台
---

* 2020-05-22 完成制造和装配
  * 调通LCD(无GUI，非DMA)
  * 添加IIC和UART通信

---

* 2020-08-07 完成Demo
  * 考试结束，开始摸鱼
  * 在VSCode中开发
  * 将LCD改为GRAM刷新
  * 添加VL53L0x示例代码
  * 发现安装上的一些小问题