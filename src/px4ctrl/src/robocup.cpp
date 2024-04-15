#include "robocup.h"
void robocup(void )
{
    switch(task_state)
    {
        case INIT:
        {
            // 代码运行，任务初始化

            // 舵机初始化
            
            // 初始化投抛数量为0
            throw_num = 0;
            // 初始化成功后进入起飞模式准备
            task_state = TAKEOFF;
            break;
        }
        case TAKEOFF:
        {
            // 起飞成功后进入悬停模式
            task_state = HOVER;
            break;
        }
        case HOVER:
        {
            // 起飞成功后，空中悬停30秒
            
            // 进入全局搜索模式
            task_state = SEARCH;
            break;
        }
        case SEARCH:
        {
            // 按指定路径进行全局搜索

            // 判断是否检测到靶标

            // 无人机飞到靶标正上方，进入投抛模式

            task_state = THROW;
            break;
        }
        case THROW:
        {
            
            // 无人机降低高度到指定位置

            // 判断已投抛数量，驱动投机到不同位置，进行物品投掷
            switch (throw_num) 
            {
                case 0:
                {
                    // 舵机控制，一段
                    throw_num++;
                    break;
                }
                case 1:
                {
                    // 舵机控制，二段
                    throw_num++;
                    break;
                }case 2:
                {
                    // 舵机控制，三段
                    throw_num++;
                    break;
                }
                default:
                    break;
            }
            // 判断已投抛数量，选择进入全局模式还是窗户模式
            if (throw_num == 3)
            {
                task_state = WINDOW;
            }
            else
            {
                task_state = HOVER;
            }
            break;
        }
        case WINDOW:
        {
            
            
            // 进入窗户，到达窗户后，进入穿越走廊模式
            task_state = CORRIDOR;
            break;
        }
        case CORRIDOR:
        {
            
            
            // 穿越走廊

            // 到达降落地标附近后开始局部搜索降落地标

            // 识别到降落地标后进入降落模式
            task_state = CORRIDOR;
            break;
        }
        case LAND:
        {
            
            // 达到降落地标上空，电机上锁，退出程序控制
            task_state = LAND;
            break;
        }
        default:
            break;
        
    }
}