#!/bin/bash


export ROSLAUNCH_SSH_UNKNOWN=1
export ROS_MASTER_URI=http://192.168.0.100:11311
export ROS_IP=192.168.0.101

source /opt/ros/indigo/setup.bash
source ~/catkin_ws/devel/setup.bash

exec "$@"
cd /home/or/catkin_ws/src/komodo-iaea-challenge/robotican_komodo/java/IAEA_Challenge/maps/currmap
rosrun map_server map_saver -f or11

sshpass -p 'or2591989' scp -r /home/or/catkin_ws/src/komodo-iaea-challenge/robotican_komodo/java/IAEA_Challenge/maps/currmap avishys_lab_ubuntu@192.168.0.100:/home/avishys_lab_ubuntu/catkin_ws/src/komodo-iaea-challenge/robotican_komodo/maps/ 

convert or11.pgm or11.png

