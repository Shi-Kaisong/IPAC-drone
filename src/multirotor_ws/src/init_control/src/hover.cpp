#include "ros/ros.h"
#include "sensor_msgs/Range.h"
#include "geometry_msgs/Twist.h"
#include "std_msgs/String.h"
#include "mavros_msgs/State.h"
#include "mavros_msgs/CommandBool.h"
#include "mavros_msgs/SetMode.h"
#include "geometry_msgs/PoseStamped.h"

mavros_msgs::State current_state;
void state_cb(const mavros_msgs::State::ConstPtr& msg)
{
    current_state = *msg;
}

float global_height = 0.0;
//高度
void get_height(const sensor_msgs::Range::ConstPtr& height)
{
    global_height = height->range;
    ROS_INFO("%.2f",global_height);
}

int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");

    //初始化
    ros::init(argc,argv,"my_takeoff");
    ros::NodeHandle nh;

    //高度订阅 
    ros::Subscriber sub = nh.subscribe<sensor_msgs::Range>
        ("/tfmini_ros_node/TFmini",10,get_height);

    ros::Subscriber state_sub = nh.subscribe<mavros_msgs::State>
        ("mavros/state", 10, state_cb);

    //模式切换
    ros::Publisher multi_cmd_vel_flu_pub = nh.advertise<geometry_msgs::Twist>
        ("/cmd_vel_flu",1);
    ros::Publisher multi_cmd_pub = nh.advertise<std_msgs::String>
        ("/cmd",3);

    ros::ServiceClient arming_client = nh.serviceClient<mavros_msgs::CommandBool>
        ("mavros/cmd/arming");
    ros::ServiceClient set_mode_client = nh.serviceClient<mavros_msgs::SetMode>
        ("mavros/set_mode");

    std_msgs::String cmd;
    geometry_msgs::Twist twist;

    mavros_msgs::CommandBool arm_cmd;
    arm_cmd.request.value = true;

    ros::Rate rate(20);
    // wait for FCU connection
    while(ros::ok() && !current_state.connected)
    {
        ros::spinOnce();
        rate.sleep();
    }

    twist.linear.x = 0.0; twist.linear.y = 0.0; twist.linear.z = atof(argv[1]);
    twist.angular.x = 0.0; twist.angular.y = 0.0;  twist.angular.z = 0.0;
    cmd.data = "HOVER";


    //send a few setpoints before starting
    for(int i = 100; ros::ok() && i > 0; --i)
    {
        multi_cmd_vel_flu_pub.publish(twist);
        multi_cmd_pub.publish(cmd);
        ros::spinOnce();
        rate.sleep();
    }

    ros::Time last_request = ros::Time::now();

    while(ros::ok())
    {
        if (global_height > atof(argv[2]))
        {
            twist.linear.x = 0.0; twist.linear.y = 0.0; twist.linear.z = 0.0;
            twist.angular.x = 0.0; twist.angular.y = 0.0;  twist.angular.z = 0.0;

            for(int i = 100; ros::ok() && i > 0; --i)
            {
                multi_cmd_vel_flu_pub.publish(twist);

                cmd.data = "HOVER";
                multi_cmd_pub.publish(cmd);

                ros::spinOnce();
                rate.sleep();
            }
            last_request = ros::Time::now();
            ROS_INFO("到达目标高度");
            
            break;
        }

        multi_cmd_vel_flu_pub.publish(twist);

        ros::spinOnce();

        rate.sleep();
    }

    ros::Duration(5.0).sleep();
    twist.linear.x = 0.0; twist.linear.y = 0.0; twist.linear.z = -0.2;
    twist.angular.x = 0.0; twist.angular.y = 0.0;  twist.angular.z = 0.0;

    while(ros::ok())
    {
        multi_cmd_vel_flu_pub.publish(twist);
        
        ROS_INFO("开始降落");

        ros::spinOnce();
        rate.sleep();
    }
    
    return 0;
}
