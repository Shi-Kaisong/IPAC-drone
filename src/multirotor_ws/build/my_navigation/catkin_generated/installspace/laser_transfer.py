#!/usr/bin/env python2
import rospy
from geometry_msgs.msg import PoseStamped
from tf2_ros import TransformListener, Buffer
from sensor_msgs.msg import Range


rospy.init_node('mavros_laser_transfer')
pose_pub = rospy.Publisher("/mavros/vision_pose/pose", PoseStamped, queue_size=1)
tfBuffer = Buffer()
tflistener = TransformListener(tfBuffer)
local_pose = PoseStamped()
local_pose.header.frame_id = 'map'
hector = PoseStamped()
height = 0


def hector_callback(data):
    global hector
    hector = data

def gazebo_model_state_callback(Range):
    global height
    height = Range.range
    
def hector_slam():
    global local_pose, height
    pose2d_sub = rospy.Subscriber("/pose", PoseStamped, hector_callback,queue_size=1)
    rate = rospy.Rate(100)  
    while not rospy.is_shutdown():
        local_pose = hector
        local_pose.pose.position.z = height
        pose_pub.publish(local_pose)
        rate.sleep()
        
    
if __name__ == '__main__':
    gazebo_model_state_sub = rospy.Subscriber("/tfmini_ros_node/TFmini", Range, gazebo_model_state_callback,queue_size=1)
    hector_slam()