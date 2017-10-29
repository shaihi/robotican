#!/bin/bash

export ROSLAUNCH_SSH_UNKNOWN=1
export ROS_MASTER_URI=http://192.168.0.102:11311
export ROS_IP=192.168.0.100

source /opt/ros/indigo/setup.bash
source ~/catkin_ws/devel/setup.bash
exec "$@"
rostopic pub /syscommand std_msgs/String "data: 'reset'"


