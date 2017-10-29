#!/bin/bash


export ROSLAUNCH_SSH_UNKNOWN=1
export ROS_MASTER_URI=http://192.168.0.102:11311
export ROS_IP=192.168.0.100

source /opt/ros/indigo/setup.bash
source ~/catkin_ws/devel/setup.bash
exec "$@"

sshpass -p 'or2591989' scp /home/or/catkin_ws/src/komodo-iaea-challenge/robotican_komodo/java/IAEA_Challenge/pos.txt avishys_lab_ubuntu@192.168.0.100:/home/avishys_lab_ubuntu/catkin_ws/src/komodo-iaea-challenge/robotican_komodo/java/IAEA_Challenge 


