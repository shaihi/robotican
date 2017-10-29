#!/bin/bash

export ROSLAUNCH_SSH_UNKNOWN=1
export ROS_MASTER_URI=http://192.168.0.102:11311
export ROS_IP=192.168.0.100

source /opt/ros/indigo/setup.bash
source ~/catkin_ws/devel/setup.bash
exec "$@"
rosnode kill /hector_mapping
kill -9 $(ps ax | grep rviz | fgrep -v grep | awk '{ print $1 }')


