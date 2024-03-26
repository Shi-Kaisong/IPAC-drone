#include <ros/ros.h>
#include <std_msgs/String.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/PoseStamped.h>
#include "stdio.h"
#include "wiringPi.h" 

// 目标位置
geometry_msgs::Pose global_pose;
bool is_arrive = false;

void get_pose(const geometry_msgs::PoseStamped::ConstPtr& pose)
{
    if(pose->pose.position.x < ( global_pose.position.x + 0.2 ) 
        && pose->pose.position.x > ( global_pose.position.x - 0.2 ))
    {
        if(pose->pose.position.y < ( global_pose.position.y + 0.2)
            && pose->pose.position.y > ( global_pose.position.y - 0.2 ))
        {
            if(pose->pose.position.z < ( global_pose.position.z + 0.1)
            && pose->pose.position.z > ( global_pose.position.z - 0.1 ))
            {
                is_arrive = true;
            }
        }
    }
    if(!is_arrive)
    {
        ROS_INFO("x:%.2f \t y:%.2f \t z:%.2f",pose->pose.position.x, pose->pose.position.y,  pose->pose.position.z);
        ROS_INFO("send_x:%.2f \t send_y:%.2f \t send_z:%.2f",global_pose.position.x, global_pose.position.y,  global_pose.position.z);
    }
}

void object(int id)
{
    if (id == 1)
    {
        for (size_t i = 0; i <5; i++)
        {
            digitalWrite(26,HIGH);  //pack_1 fall
            delayMicroseconds(750);  
            digitalWrite(26,LOW);  
            delayMicroseconds(19250);  
        }
        ROS_INFO("第一个投放点");
    }
    else if (id == 2)
    {
        for (size_t i = 0; i <5; i++)
        {
            digitalWrite(26,HIGH);  //pack_2 fall
            delayMicroseconds(1100);  
            digitalWrite(26,LOW);  
            delayMicroseconds(18900);  
        } 
        ROS_INFO("第二个投放点");
    }
    else if (id == 3)
    {
        for (size_t i = 0; i <5; i++) //pack_3 fall
        {
            digitalWrite(26,HIGH);  
            delayMicroseconds(1500);  
            digitalWrite(26,LOW);  
            delayMicroseconds(18500);  
        }
        ROS_INFO("第三个投放点");
    }
    else if (id == 0)
    {
        for (size_t i = 0; i <5; i++) //zero
        {
            digitalWrite(26,HIGH);  
            delayMicroseconds(500);  
            digitalWrite(26,LOW);  
            delayMicroseconds(19500);  
        }
        ROS_INFO("初始化");
    }
    
}

int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");
    
    ros::init(argc, argv,"pose_control");
    ros::NodeHandle nh;

    ros::Publisher local_pos_pub = nh.advertise<geometry_msgs::Pose>
        ( "/cmd_pose_enu", 10);

    ros::Subscriber pose_sub = nh.subscribe<geometry_msgs::PoseStamped>
        ( "/mavros/local_position/pose", 10, get_pose);

    ros::Publisher multi_cmd_pub = nh.advertise<std_msgs::String>
        ("/cmd",3);

    wiringPiSetup();
    pinMode(26,OUTPUT);
    //the setpoint publishing rate MUST be faster than 2Hz
    ros::Rate rate(20.0);

    int point_num = atoi(argv[2]);  
    int not_put_id = atoi(argv[3]);
    int put_num[4];

    int count_num = 1;
    for (int j = 0; j <4; j++)
    {
        if (j != (not_put_id - 2))
        {
            put_num[j] = count_num;
            count_num++;
        }
        else
            put_num[j] = -1; //do nothing 
    }
    object(0);

    while(ros::ok())
    {
        global_pose.position.x = 0.0;
        global_pose.position.y = 0.0;
        global_pose.position.z = 1.2;
        ROS_INFO("Sending goal");
        // go
        while (!is_arrive && ros::ok())
        {
            local_pos_pub.publish(global_pose);
            ros::spinOnce();
            rate.sleep();
        }
        is_arrive = false;
        ros::Duration(10).sleep();
        for (int i = 1; i <= point_num && ros::ok(); i++)
        {
            // 目标位置
            global_pose.position.x = atof(argv[i+3]);
            global_pose.position.y = atof(argv[i+3+point_num]);
            global_pose.position.z = atof(argv[1]);
            ROS_INFO("Sending goal");
            // go
            while (!is_arrive && ros::ok())
            {
                local_pos_pub.publish(global_pose);
                ros::spinOnce();
                rate.sleep();
            }
            is_arrive = false;

            ros::Duration(6).sleep();
            if (i == 1 || i == 6 || i == 7)
                continue;
            // low
            global_pose.position.z = 0.6;
            while (!is_arrive && ros::ok())
            {
                local_pos_pub.publish(global_pose);
                ros::spinOnce();
                rate.sleep();
            }
            is_arrive = false;
            ros::Duration(3).sleep();
            // 投放
            if (i > 1 && i < 6)
                object(put_num[i-2]);
            ros::Duration(4).sleep();
            // high
            global_pose.position.z = atof(argv[1]);
            while (!is_arrive && ros::ok())
            {
                local_pos_pub.publish(global_pose);
                ros::spinOnce();
                rate.sleep();
            }
            is_arrive = false;
            ros::Duration(4).sleep();
        }
        // land 
        global_pose.position.z = 0.2;
        while (!is_arrive && ros::ok())
        {
            local_pos_pub.publish(global_pose);
            ros::spinOnce();
            rate.sleep();
        }
        ros::Duration(3).sleep();

        std_msgs::String cmd;
        cmd.data = "DISARM";
        while (ros::ok())
        {
            multi_cmd_pub.publish(cmd);
            ros::spinOnce();
            rate.sleep();
        }
    }
    return 0;
}