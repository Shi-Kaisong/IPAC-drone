#include <ros/ros.h>
#include <std_msgs/String.h>
#include <geometry_msgs/Pose.h>
#include <mavros_msgs/State.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>

int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");

    ros::init(argc, argv,  "init_control");
    ros::NodeHandle nh;

    ros::Publisher multi_cmd_pub = nh.advertise<std_msgs::String>
             ("/cmd",3);

    //the setpoint publishing rate MUST be faster than 2Hz
    ros::Rate rate(20.0);

    std_msgs::String cmd;
    cmd.data = "DISARM";
    while (ros::ok())
    {
        multi_cmd_pub.publish(cmd);
        ROS_INFO("%s",cmd.data.c_str());
        ros::spinOnce();
        rate.sleep();
    }
    return 0;
}