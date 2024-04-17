#include "robocup.h"


Drone drone;
void Drone::robocup(void )
{
    ros::Time now_time = ros::Time::now();
    switch(task_state)
    {
        case INIT:
        {
            // 代码运行，任务初始化
            target_state = NOT;
            
            // 加载全局搜索路径
            setpoint_position = key_point.Start_point;
            // 舵机初始化
            steer_state = Init;
            // 初始化投抛数量为0
            throw_num = 0;
            // 初始化成功后进入起飞模式准备
            if (ros::ok() && target_state == NOT 
                        && setpoint_position == key_point.Start_point 
                        && steer_state == Init
                        && throw_num == 0)
            {
                task_state = TAKEOFF;
                ROS_INFO("[robocup] From INIT to TAKEOFF!");
            }
            
            break;
        }
        case TAKEOFF:
        {
            if (ros::ok() && Is_arrive( key_point.Hover_point, odom_data))
            {
                task_state = HOVER;
                ROS_INFO("[robocup] From TAKEOFF to HOVER!");
                hover_time = ros::Time::now();
                break;
            }
            // 起飞成功后进入悬停模式
            else
            {
                ROS_INFO("[robocup] not arrived hover position!");

                break;
            }
            
        }
        case HOVER:
        {

            // 起飞成功后，空中悬停30秒
            if (now_time - hover_time == 30)
            {
                // 进入全局搜索模式
                task_state = SEARCH;
                ROS_INFO("[robocup] From HOVER to SEARCH!");
                flag.key_flag = 0;
                break;
            }
            else
            {
                ROS_INFO("[robocup] hovering! please wait...");
                break;
            }
            
            
        }
        case SEARCH:
        {
            // 按指定路径进行全局搜索
            if (ros::ok && !target_state)
            {
                switch (flag.key_flag ) 
                {
                    case 0:
                    {
                        setpoint_position = key_point.key1;
                        if (Is_arrive(key_point.key1, odom_data))
                        {
                            flag.key_flag = 1;
                        }
                        break;
                    }
                    case 1:
                    {
                        setpoint_position = key_point.key2;
                        if (Is_arrive(key_point.key2, odom_data))
                        {
                            flag.key_flag = 2;
                        }
                        break;
                    }
                    case 2:
                    {
                        setpoint_position = key_point.key3;
                        if (Is_arrive(key_point.key3, odom_data))
                        {
                            flag.key_flag = 3;
                        }
                        break;
                    }
                    case 3:
                    {
                        task_state = THROW;
                        ROS_INFO("[robocup] From SEARCH to THROW!");
                    }
                    default:
                        break;
                    
                }
            }
            else
            {
                // 无人机飞到靶标正上方，进入投抛模式并使无人机降低高度到指定位置
                setpoint_position = yolo_turn_position(setpoint_position, yolo_data.x, yolo_data.y);
                if(Is_arrive(setpoint_position, odom_data))
                {
                    task_state = THROW;
                    ROS_INFO("[robocup] From SEARCH to THROW!");
                }
                else
                {
                    ROS_INFO("[robocup] yolo turning!");
                }
                
                
            }
            

            
            break;
        }
        case THROW:
        {
            
            

            // 判断已投抛数量，驱动投机到不同位置，进行物品投掷
            switch (flag.throw_num) 
            {
                case 0:
                {
                    // 舵机控制，初始化
                    
                    break;
                }
                case 1:
                {
                    // 舵机控制，一段
                    flag.throw_num++;
                    break;
                }case 2:
                {
                    // 舵机控制，二段
                    flag.throw_num++;
                    break;
                }
                case 3:
                {
                    // 舵机控制，三段
                    flag.throw_num++;
                    break;
                }
                default:
                    break;
            }
            control_steer(flag.throw_num);
            // 判断已投抛数量，选择进入全局模式还是窗户模式
            if (flag.throw_num == 3)
            {
                task_state = WINDOW;
            }
            else
            {
                task_state = SEARCH;
            }
            break;
        }
        case WINDOW:
        {
            
            setpoint_position = key_point.Window_point;
            if (Is_arrive(key_point.Window_point, odom_data))
            {
                task_state = CORRIDOR;
                ROS_INFO("[robocup] arrvied window!")
                // 进入窗户，到达窗户后，进入穿越走廊模式
                ROS_INFO("[robocup] From WINDOW to CORRIDOR!");
                
            }
            else
            {
                ROS_INFO("[robocup] entering window!");
                
            }
            
            
            break;
        }
        case CORRIDOR:
        {
            
            
            // 穿越走廊
            setpoint_position = key_point.Land_point;
            if (Is_arrive(key_point.Land_point, odom_data))
            {
                task_state = LAND;
                ROS_INFO("[robocup] have crossed corridor!")
                ROS_INFO("[robocup] From CORRIDOR to LAND!");
                
            }
            else
            {
                ROS_INFO("[robocup] crossing corridor!");
                
            }
            
            
            break;
        }
        case LAND:
        {
            // 到达降落地标附近后开始局部搜索降落地标
            setpoint_position = yolo_turn_position(setpoint_position, yolo_data.x, yolo_data.y);
            if(Is_arrive(setpoint_position, odom_data))
            {
                // 识别到降落地标后进入降落模式
                
                ROS_INFO("[robocup] start to land" );
                task_state = AUTO_land;
            }
            else
            {
                ROS_INFO("[robocup] yolo turning!");
            }
            
            break;
        }
        
        default:
            break;
        
    }
    // 发布期望坐标到ego
    publish_setpoint_position(setpoint_position, now_time);
    
    ros::Time last_time = now_time;
}
void Drone::config_from_ros_handle(const ros::NodeHandle &nh)
{
    read_essential_param(nh, "key_point/Start_point/pose/position/x", key_point.Start_point.pose.position.x);
    read_essential_param(nh, "key_point/Start_point/pose/position/y", key_point.Start_point.pose.position.y);
    read_essential_param(nh, "key_point/Start_point/pose/position/z", key_point.Start_point.pose.position.z);
    read_essential_param(nh, "key_point/Start_point/pose/orientation/x", key_point.Start_point.pose.orientation.x);
    read_essential_param(nh, "key_point/Start_point/pose/orientation/y", key_point.Start_point.pose.orientation.y);
    read_essential_param(nh, "key_point/Start_point/pose/orientation/z", key_point.Start_point.pose.orientation.z);
    read_essential_param(nh, "key_point/Start_point/pose/orientation/w", key_point.Start_point.pose.orientation.w);
    read_essential_param(nh, "key_point/Land_point/pose/position/x", key_point.Land_point.pose.position.x);
    read_essential_param(nh, "key_point/Land_point/pose/position/y", key_point.Land_point.pose.position.y);
    read_essential_param(nh, "key_point/Land_point/pose/position/z", key_point.Land_point.pose.position.z);
    read_essential_param(nh, "key_point/Land_point/pose/orientation/x", key_point.Land_point.pose.orientation.x);
    read_essential_param(nh, "key_point/Land_point/pose/orientation/y", key_point.Land_point.pose.orientation.y);
    read_essential_param(nh, "key_point/Land_point/pose/orientation/z", key_point.Land_point.pose.orientation.z);
    read_essential_param(nh, "key_point/Land_point/pose/orientation/w", key_point.Land_point.pose.orientation.w);
    read_essential_param(nh, "key_point/Window_point/pose/position/x", key_point.Window_point.pose.position.x);
    read_essential_param(nh, "key_point/Window_point/pose/position/y", key_point.Window_point.pose.position.y);
    read_essential_param(nh, "key_point/Window_point/pose/position/z", key_point.Window_point.pose.position.z);
    read_essential_param(nh, "key_point/Window_point/pose/orientation/x", key_point.Window_point.pose.orientation.x);
    read_essential_param(nh, "key_point/Window_point/pose/orientation/y", key_point.Window_point.pose.orientation.y);
    read_essential_param(nh, "key_point/Window_point/pose/orientation/z", key_point.Window_point.pose.orientation.z);
    read_essential_param(nh, "key_point/Window_point/pose/orientation/w", key_point.Window_point.pose.orientation.w);
    read_essential_param(nh, "key_point/Hover_point/pose/position/x", key_point.Window_point.pose.position.x);
    read_essential_param(nh, "key_point/Hover_point/pose/position/y", key_point.Hover_point.pose.position.y);
    read_essential_param(nh, "key_point/Hover_point/pose/position/z", key_point.Hover_point.pose.position.z);
    read_essential_param(nh, "key_point/Hover_point/pose/orientation/x", key_point.Hover_point.pose.orientation.x);
    read_essential_param(nh, "key_point/Hover_point/pose/orientation/y", key_point.Hover_point.pose.orientation.y);
    read_essential_param(nh, "key_point/Hover_point/pose/orientation/z", key_point.Hover_point.pose.orientation.z);
    read_essential_param(nh, "key_point/Hover_point/pose/orientation/w", key_point.Hover_point.pose.orientation.w);
    read_essential_param(nh, "key_point/key1/pose/position/x", key_point.key1.pose.position.x);
    read_essential_param(nh, "key_point/key1/pose/position/y", key_point.key1.pose.position.y);
    read_essential_param(nh, "key_point/key1/pose/position/z", key_point.key1.pose.position.z);
    read_essential_param(nh, "key_point/key1/pose/orientation/x", key_point.key1.pose.orientation.x);
    read_essential_param(nh, "key_point/key1/pose/orientation/y", key_point.key1.pose.orientation.y);
    read_essential_param(nh, "key_point/key1/pose/orientation/z", key_point.key1.pose.orientation.z);
    read_essential_param(nh, "key_point/key1/pose/orientation/w", key_point.key1.pose.orientation.w);
    read_essential_param(nh, "key_point/key2/pose/position/x", key_point.key2.pose.position.x);
    read_essential_param(nh, "key_point/key2/pose/position/y", key_point.key2.pose.position.y);
    read_essential_param(nh, "key_point/key2/pose/position/z", key_point.key2.pose.position.z);
    read_essential_param(nh, "key_point/key2/pose/orientation/x", key_point.key2.pose.orientation.x);
    read_essential_param(nh, "key_point/key2/pose/orientation/y", key_point.key2.pose.orientation.y);
    read_essential_param(nh, "key_point/key2/pose/orientation/z", key_point.key2.pose.orientation.z);
    read_essential_param(nh, "key_point/key2/pose/orientation/w", key_point.key2.pose.orientation.w);
    read_essential_param(nh, "key_point/key3/pose/position/x", key_point.key3.pose.position.x);
    read_essential_param(nh, "key_point/key3/pose/position/y", key_point.key3.pose.position.y);
    read_essential_param(nh, "key_point/key3/pose/position/z", key_point.key3.pose.position.z);
    read_essential_param(nh, "key_point/key3/pose/orientation/x", key_point.key3.pose.orientation.x);
    read_essential_param(nh, "key_point/key3/pose/orientation/y", key_point.key3.pose.orientation.y);
    read_essential_param(nh, "key_point/key3/pose/orientation/z", key_point.key3.pose.orientation.z);
    read_essential_param(nh, "key_point/key3/pose/orientation/w", key_point.key3.pose.orientation.w);
    read_essential_param(nh, "Steer_angle/group", steer_angle.group);
    read_essential_param(nh, "Steer_angle/angle_1", steer_angle.angle_1);
    read_essential_param(nh, "Steer_angle/angle_2", steer_angle.angle_2);
    read_essential_param(nh, "Steer_angle/angle_3", steer_angle.angle_3);
    read_essential_param(nh, "Steer_angle/angle_4", steer_angle.angle_4);
}
Drone::Drone()
{
    task_state = INIT;
}



Drone::~Drone()
{
    
}



geometry_msgs::PoseStamped Drone::yolo_turn_position(geometry_msgs::PoseStamped current, float yolo_x, float yolo_y )
{
    // 
    current.pose.position.x = current.pose.position.x + yolo_x;
    current.pose.position.y = current.pose.position.y + yolo_y;
    current.pose.position.z = 0.6;

    return current;
}

void Drone::publish_setpoint_position(geometry_msgs::PoseStamped &pos, ros::Time &stamp)
{
    geometry_msgs::PoseStamped msg;
    msg.header.stamp = stamp;
    msg.header.frame_id = 'world';
    msg.pose.position.x = pos.pose.position.x;
    msg.pose.position.y = pos.pose.position.y;
    msg.pose.position.z = pos.pose.position.z;
    msg.pose.orientation.x = pos.pose.orientation.x;
    msg.pose.orientation.y = pos.pose.orientation.y;
    msg.pose.orientation.z = pos.pose.orientation.z;
    msg.pose.orientation.w = pos.pose.orientation.w;
    position_pub.publish(msg);
}

bool Drone::Is_arrive(geometry_msgs::PoseStamped point, nav_msgs::Odometry Odometry)
{
    if (((Odometry.pose.pose.position.x < (point.pose.position.x + error)) || (Odometry.pose.pose.position.x > (point.pose.position.x - error)))
        && ((Odometry.pose.pose.position.y < (point.pose.position.y + error)) || (Odometry.pose.pose.position.y > (point.pose.position.y - error)))
        && ((Odometry.pose.pose.position.z < (point.pose.position.z + error)) || (Odometry.pose.pose.position.z > (point.pose.position.z - error))))

    {
        return true;
    }
    else
    {
        return false;
    }
}


void Drone::yolo_data_deal(robocup_yolo::yolo pmsgs)
{
    yolo_data = pmsgs;
    if (yolo_data.target == "TENT")
    {
        target_state = TENT;
    }
    else if (yolo_data.target == "PILLBOX")
    {
        target_state = PILLBOX;
    }
    else if (yolo_data.target == "BRIDGE")
    {
        target_state = PILLBOX;
    }
    else if (yolo_data.target == "CAR")
    {
        target_state = CAR;
    }
    else if (yolo_data.target == "TANK")
    {
        target_state = TANK;
    }
    else if (yolo_data.target == "H")
    {
        target_state = H;
    }
    else 
    {
        target_state = NOT;
    }
}

void Drone::control_steer(int throw_nums)
{
    control_Actual.group_mix = 2;
    if (throw_nums == 0)
    {
        claw_control_mix.controls[steer_angle.group] = steer_angle.angle_1;
    }
    else if (throw_nums == 1)
    {
        claw_control_mix.controls[steer_angle.group] = steer_angle.angle_2;
    }
    else if (throw_nums == 2)
    {
        claw_control_mix.controls[steer_angle.group] = steer_angle.angle_3;
    }
    else
    {
        claw_control_mix.controls[steer_angle.group] = steer_angle.angle_4;
    }
        
    steer_pub.publish(control_Actual);
    
}