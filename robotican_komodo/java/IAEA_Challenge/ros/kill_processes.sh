#!/bin/bash

source /opt/ros/indigo/setup.bash
source ~/catkin_ws/devel/setup.bash

killall rosmaster
killall gzserver
killall gzclient

