# IPAC—Drone 

[toc]

### 软硬件环境介绍：

本项目采用Pixhawk v6c飞控，jetson orin nx 16G作为机载电脑，该机载电脑搭配ubuntu20.04系统，采用的cuda是11.4版本，无人机的轴距是330mm。

## 一、机载电脑环境部署

### （一）ROS安装
推荐使用鱼香ROS的一键安装，安装命令如下：
```
wget http://fishros.com/install -O fishros && . fishros
```
### （二）MAVROS安装

```
sudo apt-get install ros-noetic-mavros
cd /opt/ros/noetic/lib/mavros
sudo ./install_geographiclib_datasets.sh
```

### （三）realsense驱动安装
#### 1、RealSense的SDK安装
更新初始化
```
sudo apt-get update && sudo apt-get upgrade && sudo apt-get dist-upgrade
```
注册服务器的公钥
```
sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-key F6E65AC044F831AC80A06380C8B3A55A6F3EFCDE || sudo apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-key F6E65AC044F831AC80A06380C8B3A55A6F3EFCDE
```
将服务器添加到存储库列表中
```
sudo add-apt-repository "deb https://librealsense.intel.com/Debian/apt-repo $(lsb_release -cs) main" -u
```
安装SDK
```
sudo apt-get install librealsense2-dkms
sudo apt-get install librealsense2-utils
```
安装dev和debug工具
```
sudo apt-get install librealsense2-dbg
```
测试安装结果
```
realsense-viewer 
```
#### 2、安装ROS版本的realsense2_camera
```
sudo apt-get install ros-noetic-realsense2-camera
sudo apt-get install ros-noetic-realsense2-description
```
### （四）clone仓库源码

+ 仓库中包含ego-plenner源码，VISN-Fusion-gpu源码

clone仓库

```
git clone https://github.com/Shi-Kaisong/IPAC-drone.git
```

```
sed -i 's/CV_FONT_HERSHEY_SIMPLEX/cv::FONT_HERSHEY_SIMPLEX/g' `grep CV_FONT_HERSHEY_SIMPLEX -rl ./`
sed -i 's/CV_LOAD_IMAGE_GRAYSCALE/cv::IMREAD_GRAYSCALE/g' `grep CV_LOAD_IMAGE_GRAYSCALE -rl ./`
sed -i 's/CV_BGR2GRAY/cv::COLOR_BGR2GRAY/g' `grep CV_BGR2GRAY -rl ./`
sed -i 's/CV_RGB2GRAY/cv::COLOR_RGB2GRAY/g' `grep CV_RGB2GRAY -rl ./`
sed -i 's/CV_GRAY2RGB/cv::COLOR_GRAY2RGB/g' `grep CV_GRAY2RGB -rl ./`
sed -i 's/CV_GRAY2BGR/cv::COLOR_GRAY2BGR/g' `grep CV_GRAY2BGR -rl ./`
sed -i 's/CV_CALIB_CB_ADAPTIVE_THRESH/cv::CALIB_CB_ADAPTIVE_THRESH/g' `grep CV_CALIB_CB_ADAPTIVE_THRESH -rl ./`
sed -i 's/CV_CALIB_CB_NORMALIZE_IMAGE/cv::CALIB_CB_NORMALIZE_IMAGE/g' `grep CV_CALIB_CB_NORMALIZE_IMAGE -rl ./`
sed -i 's/CV_CALIB_CB_FILTER_QUADS/cv::CALIB_CB_FILTER_QUADS/g' `grep CV_CALIB_CB_FILTER_QUADS -rl ./`
sed -i 's/CV_CALIB_CB_FAST_CHECK/cv::CALIB_CB_FAST_CHECK/g' `grep CV_CALIB_CB_FAST_CHECK -rl ./`
sed -i 's/CV_ADAPTIVE_THRESH_MEAN_C/cv::ADAPTIVE_THRESH_MEAN_C/g' `grep CV_ADAPTIVE_THRESH_MEAN_C -rl ./`
sed -i 's/CV_THRESH_BINARY/cv::THRESH_BINARY/g' `grep CV_THRESH_BINARY -rl ./`
sed -i 's/CV_SHAPE_CROSS/cv::MORPH_CROSS/g' `grep CV_SHAPE_CROSS -rl ./`
sed -i 's/CV_SHAPE_RECT/cv::MORPH_RECT/g' `grep CV_SHAPE_RECT -rl ./`
sed -i 's/CV_TERMCRIT_EPS/cv::TermCriteria::EPS/g' `grep CV_TERMCRIT_EPS -rl ./`
sed -i 's/CV_TERMCRIT_ITER/cv::TermCriteria::MAX_ITER/g' `grep CV_TERMCRIT_ITER -rl ./`
sed -i 's/CV_RETR_CCOMP/cv::RETR_CCOMP/g' `grep CV_RETR_CCOMP -rl ./`
sed -i 's/CV_CHAIN_APPROX_SIMPLE/cv::CHAIN_APPROX_SIMPLE/g' `grep CV_CHAIN_APPROX_SIMPLE -rl ./`
sed -i 's/CV_AA/cv::LINE_AA/g' `grep CV_AA -rl ./`
```
### （五）Ceres安装
安装依赖库
```
sudo apt-get install liblapack-dev libsuitesparse-dev libgflags-dev 
sudo apt-get install libgoogle-glog-dev libgtest-dev
sudo apt-get install libcxsparse3
```
进入安装包
```
cd ~/IPAC-drone/doc/ceres-solver-1.14.0
```
建立编译安装包
```
sudo mkdir build
```
进入编译安装包
```
cd build
```
预编译
```
sudo cmake ..
```
编译安装
```
sudo make
sudo make install
```



### （六）OpenCv4.6.0和cv_bridge安装（gpu)

#### 1、opencv安装

移动至指定目录

```
cd ~/IPAC-drone/doc/
sudo mv ./opencv-4.6.0/ /usr/local/
sudo mv ./opencv_contirb-4.6.0/ /usr/local/
cd /usr/local/opencv-4.6.0/
```

确定安装路径，我的是CMAKE_INSTALL_PREFIX=/usr/local

开始编译

```
sudo mkdir build && cd build
sudo cmake -D CMAKE_BUILD_TYPE=RELEASE \
        -D CMAKE_INSTALL_PREFIX=/usr/local \
        -D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib-4.6.0/modules \
        -D WITH_CUDA=ON \
        -D CUDA_ARCH_BIN=8.7 \
        -D CUDA_ARCH_PTX="" \
        -D ENABLE_FAST_MATH=ON \
        -D CUDA_FAST_MATH=ON \
        -D WITH_CUBLAS=ON \
        -D WITH_LIBV4L=ON \
        -D WITH_GSTREAMER=ON \
        -D WITH_GSTREAMER_0_10=OFF \
        -D WITH_QT=ON \
        -D WITH_OPENGL=ON \
        -D CUDA_NVCC_FLAGS="--expt-relaxed-constexpr" \
        -D WITH_TBB=ON \
        -D CUDA_SDK_ROOT_DIR=/usr/local/cuda-11.4/ \
        ..
sudo make
sudo make install
```

#### 2、cv_bridge配置

创建cv_bridge专属的工作空间

```
mkdir cv_bridge_460/src
cd ~/cv_bridge_460/src
```

将cv_bridge移动到专属工作空间

```
cp ~/IPAC-drone/doc/cv_bridge ~/cv_bridge_460/src
```


编译

```
cd ~/cv_bridge_460/
catkin_make
```

### （七）添加$$~/.bashrc$$环境

添加工作空间的环境

```
sudo vi ~/.bashrc
```

在$$~/.bashrc$$文件中添加以下命令

```
export PATH=/usr/local/cuda-11.4/bin:$PATH
export LD_LIBRARY_PATH=/usr/local/cuda-11.4/lib64:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=/home/nvidia/catkin_plan/devel/lib/pkgconfig:/home/nvidia/catkin_ws/devel/lib/pkgconfig:/home/nvidia/cv_bridge_460/devel/lib/pkgconfig:/opt/ros/noetic/lib/pkgconfig:/opt/ros/noetic/lib/aarch64-linux-gnu/pkgconfig:/usr/local/lib/pkgconfig:/usr/local/opencv-4.6.0/lib/pkgconfig

source ~/cv_bridge_460/devel/setup.bash
source ~/IPAC-drone/devel/setup.bash
```

### （八）编译项目代码

```
cd ~/IPAC-drone/
catkin_make
```

### （九）ROS主从机设置

当主从机处于同一局域网内时，可通过以下方式配置ROS主从机，进行主从机通信。

+ 注意：以下内容中涉及到远程桌面的，统一使用ROS主从机实现

+ 主机

`~/.bashrc`文件中添加以下内容：

```
export ROS_HOSTNAME=主机hostname
export ROS_MASTER_URI=http://主机IP:11311
export ROS_IP=主机IP
```

`/etc/hosts`文件中添加以下内容：

```
从机IP 从机hostname
主机IP 主机hostname
```

+ 从机

`~/.bashrc`文件中添加以下内容：

```
export ROS_HOSTNAME=从机hostname
export ROS_MASTER_URI=http://主机IP:11311
export ROS_IP=主机IP
```

`/etc/hosts`文件中添加以下内容：

```
从机IP 从机hostname
主机IP 主机hostname
```





## 二、参数修改

### （一）MAVROS

修改MAVROS中launch文件中的设备号和波特率：

```
roscd mavros
cd launch
sudo vi px4.launch
## 修改其中的端口号与波特率
```

### （二）realsense2_camera（使用D435i相机）

打开相机的双目数据

```
roscd realsense2_camera
cd launch
sudo vi rs_camere.launch
## 将”enable_infra1"和"infra2"两个参数设置为"true"
```

### （三）修改D435i内参



### （四）Ego-Planner代码框架与参数介绍

* `src/planner/plan_manage/launch/single_run_in_exp.launch`下的：
  * `map_size`：当你的地图大小较大时需要修改，注意目标点不要超过map_size/2
  * `fx/fy/cx/cy`：修改为你的深度相机的实际内参（下一课有讲怎么看）
  * `max_vel/max_acc`：修改以调整最大速度、加速度。速度建议先用0.5试飞，最大不要超过2.5，加速度不要超过6
  * `flight_type`：1代表rviz选点模式，2代表waypoints跟踪模式
* `src/planner/plan_manage/launch/advanced_param_exp.xml`下的：
  * `resolution`：代表栅格地图格点的分辨率，单位为米。越小则地图越精细，但越占内存。最小不要低于0.1
  * `obstacles_inflation`：代表障碍物膨胀大小，单位为米。建议至少设置为飞机半径（包括螺旋桨、桨保）的1.5倍以上，但不要超过`resolution`的4倍。如果飞机轴距较大，请相应改大`resolution`

### （五）PX4-ctrl框架介绍（此处来自高飞的fest-drone-250）这里是我们需要自己编写的重点

* `src/px4ctrl/config/ctrl_param_fpv.yaml`下的：
  * `mass`：修改为无人机的实际重量
  * `hover_percent`：修改为无人机的悬停油门，可以通过px4log查看，具体可以参考[文档](https://www.bookstack.cn/read/px4-user-guide/zh-log-flight_review.md) 如果你的无人机是和课程的一模一样的话，这项保持为0.3即可。如果更改了动力配置，或重量发生变化，或轴距发生变化，都请调整此项，否则自动起飞时会发生无法起飞或者超调严重的情况。
  * `gain/Kp,Kv`：即PID中的PI项，一般不用太大改动。如果发生超调，请适当调小。如果无人机响应较慢，请适当调大。
  * `rc_reverse`：这项使用乐迪AT9S的不用管。如果在第十一课的自动起飞中，发现飞机的飞行方向与摇杆方向相反，说明需要修改此项，把相反的通道对应的值改为true。其中throttle如果反了，实际实验中会比较危险，建议在起飞前就确认好，步骤为：
    * `roslaunch mavros px4.launch`
    * `rostopic echo /mavros/rc/in`
    * 打开遥控器，把遥控器油门从最低满满打到最高
    * 看echo出来的消息里哪项在缓慢变化（这项就是油门通道值），并观察它是不是由小变大
    * 如果是由小变大，则不需要修改throttle的rc_reverse，反之改为true
    * 其他通道同理

### （六）VINS的参数设置与外参标定

* 检查飞控mavros连接正常
  * `ls /dev/tty*`，确认飞控的串口连接正常。一般是`/dev/ttyACM0`
  * `sudo chmod 777 /dev/ttyACM0`，为串口附加权限
  * `roslaunch mavros px4.launch`
  * `rostopic hz /mavros/imu/data_raw`，确认飞控传输的imu频率在200hz左右
* 检查realsense驱动正常
  * `roslaunch realsense2_camera rs_camera.launch`
  * 进入远程桌面，`rqt_image_view`
  * 查看`/camera/infra1/image_rect_raw`,`/camera/infra2/image_rect_raw`,`/camera/depth/image_rect_raw`话题正常
* VINS参数设置
  * 进入`VINS_Fusion-gpu/config/`
  
  * 驱动realsense后，`rostopic echo /camera/infra1/camera_info`，把其中的K矩阵中的fx,fy,cx,cy填入`left.yaml`和`right.yaml`
  
  * 在home目录创建`vins_output`文件夹
  
  * 修改`ipac-drone-330.yaml`的`body_T_cam0`和`body_T_cam1`的`data`矩阵的第四列为你的无人机上的相机相对于飞控的实际外参，单位为米，顺序为x/y/z，第四项是1，不用改
* VINS外参精确自标定
  * `sh shfiles/rspx4.sh`
  * `rostopic echo /vins_fusion/imu_propagate`
  * 拿起飞机沿着场地尽量缓慢地行走，场地内光照变化不要太大，灯光不要太暗，不要使用会频闪的光源，尽量多放些杂物来增加VINS用于匹配的特征点
  * 把`vins_output/extrinsic_parameter.txt`里的内容替换到`ipac-drone-330.yaml`的`body_T_cam0`和`body_T_cam1`
  * 重复上述操作直到走几圈后VINS的里程计数据偏差收敛到满意值（一般在0.3米内）
* 建图模块验证
  * `sh shfiles/rspx4.sh`
  * `roslaunch ego_planner single_run_in_exp.launch`
  * 进入远程桌面 `roslaunch ego_planner rviz.launch`

