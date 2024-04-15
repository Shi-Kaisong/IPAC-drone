#ifndef ROBOCUP_H
#define ROBOCUP_H
#include <ros/ros.h>
#include <ros/assert.h>
#include <geometry_msgs/PoseStamped.h>
#include <nav_msgs/Odometry.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/CommandLong.h>
#include <mavros_msgs/CommandBool.h>
#include "input.h"
#include "controller.h"

int throw_num;
void robocup(void );
// 任务状态量
enum Task_state
{
    INIT = 1,       // 初始化
    TAKEOFF,        // 起飞
    LAND,           // 着陆
    HOVER,          // 悬停
    THROW,          // 投抛
    SEARCH,         // 全局搜索
    WINDOW,         // 进入走廊
    CORRIDOR,       // 穿越走廊
};

Task_state task_state;
#endif