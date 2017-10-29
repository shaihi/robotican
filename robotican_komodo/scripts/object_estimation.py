#!/usr/bin/python
import math
import rospy
from sensor_msgs.msg import LaserScan
from nav_msgs.msg import Odometry
from geometry_msgs.msg import Pose2D
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as anim
import matplotlib.image as mpimg
import copy as copy_module
import tf
import time

class location:
  def __init__(self,x,y,theta,time):
    self.x = x
    self.y = y
    self.theta = theta
    self.time = time
    
    
#globals
laser_info = LaserScan()
num = 0





#callbacks

    
    


def laser_callback(msg):
  global laser_info
  global scans
  global angels
  global num
  
  scan = np.copy(msg.ranges)
  angle_increment = msg.angle_increment
  x = np.copy(scan)
  y = np.copy(scan)
  k = 0;
  for i in range(0,len(scan)):
    angels = laser_info.angle_min + i*angle_increment
    if scan[i] < 2.5 and scan[i] >0.5:
      x[k] =  (math.cos(angels))*scan[i]
      y[k] =  (math.sin(angels))*scan[i]
      k = k +1
  k_max = k
  print "Start ---"
  print k_max, num
  num = num + 1
  
  file = open("aaa.txt", "a")
  
  for i in range(0,k_max):
    file.write("{}, {},{}\n".format(i, x[i], y[i]))
  #print ("x= {} y={} theta={}".format(x,y,angels))
  file.close()
  
def main():
    rospy.init_node('object_estimation')
    rospy.Subscriber("/scan", LaserScan, laser_callback)
    rate = rospy.Rate(100) # 100hz
    #while not rospy.is_shutdown():
    rospy.spin()
  

    
if __name__ == '__main__':
    main()
    