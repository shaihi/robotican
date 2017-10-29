#!/usr/bin/env bash
export ROS_MASTER_URI=http://192.168.0.100:11311
export ROS_IP=192.168.0.100
export ROSLAUNCH_SSH_UNKNOWN=1
. /home/or/catkin_ws/devel/setup.sh
#source /opt/ros/indigo/setup.bash
exec "$@"