#include <ros/ros.h>
#include <std_msgs/String.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/PoseStamped.h>

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

int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");
    
    ros::init(argc, argv,"all_control");
    ros::NodeHandle nh;

    ros::Publisher local_pos_pub = nh.advertise<geometry_msgs::Pose>
        ( "/cmd_pose_enu", 10);

    ros::Subscriber pose_sub = nh.subscribe<geometry_msgs::PoseStamped>
        ( "/mavros/local_position/pose", 10, get_pose);

    ros::Publisher multi_cmd_pub = nh.advertise<std_msgs::String>
        ("/cmd",3);

    //the setpoint publishing rate MUST be faster than 2Hz
    ros::Rate rate(20.0);

    // 移动坐标数
    int point_num = atoi(argv[2]); 

    while(ros::ok())
    {
        for (int i = 1; i <= point_num && ros::ok(); i++)
        {
            // 目标位置
            global_pose.position.x = atof(argv[i+2]);
            global_pose.position.y = atof(argv[i+2+point_num]);
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