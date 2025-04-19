# 相机驱动说明



# 时间共享方法

1 激光雷达时间戳读取与时间共享代码流程


在lds_lidar.h文件 Lds类中声明时间信息处理指针

```
  bool enable_timesync_;
  TimeSync *timesync_;
  TimeSyncConfig timesync_config_;
```

## 激光雷达/相机时间戳同步问题

[ref: 激光雷达与相机标定的时间戳同步问题 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/191372189)

1. 相机特性 - 曝光和读出
   交叠曝光和非交叠曝光
2. 图像采集：触发模式（外部输入）
   相机触发模式：外部触发和内部触发
   硬件触发：外部设备通过相机I/O接口连接，外部脉冲信号给到相机进行采集图像，本质是直接对相机内部寄存器读写

   ![1745072348912](https://file+.vscode-resource.vscode-cdn.net/e%3A/DATA/WORK/github/LiDAR_Camera_SLAM_Handhold/mvs_ros_pkg/image/camera_driver_readme/1745072348912.png)

   海康相机有1个光耦隔离输入Line0+，1个可配置输入输出Line2+，可选择一个配置为输入信号
3. 触发输出

   相机触发输出信号为开关信号，可用于控制报警灯、光源、PLC等外部设备。触发输出信号可**通过Strobe 信号实现**


## 其他方案 - 开源计划 - SLAM传感器时间同步问题

该博主研发了一款可以拓展且高度集成化的时间同步设备

ref: [【开源计划】SLAM传感器时间同步问题-从软同步到硬同步 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/27516768111)
