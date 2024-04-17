#ifndef ROBOCUP_H
#define ROBOCUP_H
#include <ros/ros.h>
#include <signal.h>
#include <ros/assert.h>
#include <geometry_msgs/PoseStamped.h>
#include <nav_msgs/Odometry.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/CommandLong.h>
#include <mavros_msgs/CommandBool.h>
#include "input.h"
#include "controller.h"
#include "robocup_yolo/yolo.h"
#define error 0.2



class Drone
{
    public:
        // 任务状态量
        enum Task_state
        {
            INIT = 1,       // 初始化
            TAKEOFF,        // 起飞
            LAND,           // 着陆
            HOVER,          // 悬停
            THROW,          // 投抛
            SEARCH,         // 全局搜索
            WINDOW,         // 进入窗户走廊
            CORRIDOR,       // 穿越走廊
            AUTO_land,
        };
        // 舵机状态量
        enum STEER_state
        {
            Init = 1, // 舵机初始化
            ONE,      // 舵机角度
            TWO,
            THREE,
        };
        // 靶标识别状态量
        enum TARGET_state
        {
            NOT = 0,  // 空
            TENT,     // 帐篷
            PILLBOX,  // 地堡
            BRIDGE,   // 桥梁
            CAR,      // 汽车
            TANK,     // 坦克
            H,        // 降落地标
            
        };
        // 关键点
        struct KEY_point
        {
            geometry_msgs::PoseStamped Start_point;
            geometry_msgs::PoseStamped Hover_point;
            geometry_msgs::PoseStamped Window_point;
            geometry_msgs::PoseStamped Land_point;
            geometry_msgs::PoseStamped key1;
            geometry_msgs::PoseStamped key2;
            geometry_msgs::PoseStamped key3;
        };
        
        struct Flag
        {
            int key_flag;
            int target_flag;
            int throw_num;
        };
        struct Steer_angle
        {
            int group;
            float angle_1;
            float angle_2;
            float angle_3;
            float angle_4;
        };
        // robocup pub start
        ros::Publisher position_pub;        // 期望坐标
        ros::Publisher RC_out_pub;          // RC_out
        ros::Publisher Mount_control_pub; 
        ros::Publisher actuator_pub;        // 混控器
        // end
        

        geometry_msgs::PoseStamped setpoint_position;

        ARGET_state target_state;
        STEER_state  steer_state;
        Task_state   task_state;
        KEY_point    key_point;
        nav_msgs::Odometry odom_data;
        Flag flag;
        Steer_angle steer_angle;
        robocup_yolo::yolo yolo_data;
        mavros_msgs::ActuatorControl control_Actual;
        
        ros::Time hover_time;
        
        void yolo_data_deal(robocup_yolo::yolo pmsgs);
        // 获取全局搜索路径坐标
        geometry_msgs::PoseStamped yolo_turn_position(geometry_msgs::PoseStamped current, float yolo_x, float yolo_y );
        void publish_setpoint_position(geometry_msgs::PoseStamped &pos, const ros::Time &stamp);
        void robocup(void );
        Drone();
        ~Drone();
        // 舵机控制
        void control_steer(int throw_nums);
        void config_from_ros_handle(const ros::NodeHandle &nh);
        // 判断无人机是否到目标点
        bool Is_arrive(geometry_msgs::PoseStamped key_point, nav_msgs::Odometry Odometry);
    private:
        template <typename TName, typename TVal>
        void read_essential_param(const ros::NodeHandle &nh, const TName &name, TVal &val)
        {
            if (nh.getParam(name, val))
            {
                // 这句话可以省略
                ROS_INFO("Read param: " << name << "to" << "val.");
            }
            else
            {
                ROS_ERROR_STREAM("Read param: " << name << " failed.");
                ROS_BREAK();
            }
        };
        
};



extern Drone drone;

#endif