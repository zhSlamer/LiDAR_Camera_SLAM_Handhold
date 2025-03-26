# LVI-SLAM 手持三维扫描仪

✍️

# 项目概述

    

# 实物展示


# 硬件方案

## 硬件清单


## 硬件接口说明


## 主控 - Jetson Xavier AGX


# 空间标定方案



# 时间同步方案

![1743000024501](image/README/connect.png)

# 软件方案

简要介绍三个代码的功能，具体说明见各代码内部md文件

FAST-LIVO2

激光雷达驱动 [lidar_driver_readme.md](livox_ros_driver\lidar_driver_readme.md)

相机驱动        [camera_driver_readme.md](mvs_ros_pkg\camera_driver_readme.md)

STM32驱动代码

# Markdown - Typora

✍️为工程代码写一份说明文档极其必要！

但是使用md文档，写说明时，难以实时预览所写工作，而且因为种种语法降低效率

推荐使用 vscode插件 **Typora**

[Github: Typora](https://github.com/cweijan/vscode-office)

# Acknowledgements🙏

感谢如下参考资料为我搭建设备提供的帮助！☺️

1. https://github.com/xuankuzcr/LIV_handhold
2. https://gitee.com/gwmunan/ros2/wikis
3. https://emoji.muan.co/

[2] Zheng, Chunran, et al. "FAST-LIVO: Fast and Tightly-coupled Sparse-Direct LiDAR-Inertial-Visual Odometry."

[1] Zheng, Chunran, et al. "FAST-LIVO2: Fast, Direct LiDAR-Inertial-Visual Odometry."
