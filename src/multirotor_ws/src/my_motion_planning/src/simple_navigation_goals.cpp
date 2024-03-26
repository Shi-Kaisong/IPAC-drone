#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/String.h>
#include <sstream>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/State.h>
#include <sensor_msgs/Range.h>
#include "mavros_msgs/CommandBool.h"
#include "mavros_msgs/SetMode.h"

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

mavros_msgs::State current_state;
void state_cb(const mavros_msgs::State::ConstPtr& msg){
  current_state = *msg;
}

float global_height = 0.0;
//高度
void get_height(const sensor_msgs::Range::ConstPtr& height)
{
    global_height = height->range;
}

int main(int argc, char* argv[])
{
  setlocale(LC_ALL,"");
  ros::init(argc, argv, "simple_navigation_goals");
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

  ros::Rate rate(20.0);

  // wait for FCU connection
  while(ros::ok() && !current_state.connected){
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
 	
 	//起飞
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
  //tell the action client that we want to spin a thread by default
  MoveBaseClient ac("move_base", true);

  //wait for the action server to come up
  while(!ac.waitForServer(ros::Duration(5.0))){
    ROS_INFO("Waiting for the move_base action server to come up");
  }

  move_base_msgs::MoveBaseGoal goal;

  //we'll send a goal to the robot to move 1 meter forward
  int point_num = atoi(argv[3]);
  for (int i = 1; i <= point_num; i++)
  {
    goal.target_pose.header.frame_id = "map";
    goal.target_pose.header.stamp = ros::Time::now();

    goal.target_pose.pose.position.x = atoi(argv[i+3]);
    goal.target_pose.pose.position.y = atoi(argv[i+3+point_num]);
    goal.target_pose.pose.orientation.w = 1.0;

    ROS_INFO("Sending goal");
    ac.sendGoal(goal);

    ac.waitForResult();

    if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
      ROS_INFO("到达Map坐标系下的(%d,%d)坐标点",atoi(argv[i+1]),atoi(argv[i+1+point_num]));
    else
      ROS_INFO("The base failed to move for some reason");
    
    ROS_WARN("开始任务,执行时间为2秒!");
    // ros::Duration(2).sleep();
    ROS_INFO("该任务执行完毕，开始下一任务!");
  }

  twist.linear.x = 0.0; twist.linear.y = 0.0; twist.linear.z = -0.2;
  twist.angular.x = 0.0; twist.angular.y = 0.0;  twist.angular.z = 0.0;
  cmd.data = "HOVER";
  
  while(ros::ok())
  {
    multi_cmd_vel_flu_pub.publish(twist);
    multi_cmd_pub.publish(cmd);

    ros::spinOnce();
    rate.sleep();
  }
  return 0;
}