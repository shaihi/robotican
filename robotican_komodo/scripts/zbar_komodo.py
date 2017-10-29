#!/usr/bin/python
from sys import argv
import zbar
import math
import rospy
from nav_msgs.msg import Odometry
import numpy as np
import tf
import time
from copy import deepcopy
import sys
import roslib
import cv2
from std_msgs.msg import String
from sensor_msgs.msg import Image
from cv_bridge import CvBridge, CvBridgeError
#globals:
robot_odometry = Odometry()
f = open("test.txt","w")
tags = []
locations = []
class image_converter:

  

  def __init__(self): 
    image_raw_topic = "/torso_camera/rgb/image_raw".format(rospy.get_name())
    self.bridge = CvBridge()
    self.image_sub = rospy.Subscriber(image_raw_topic,Image,self.callback)
    self.odometry_sub = rospy.Subscriber("/mobile_base_controller/odom", Odometry, self.Odometry_callback)
     
    
    # create a Processor
    
    
  def callback(self,data):
    try:
      cv_image = self.bridge.imgmsg_to_cv2(data, "bgr8")
    except CvBridgeError as e:
      print(e)

    scanner = zbar.ImageScanner()
    # configure the Processor
    scanner.parse_config('enable')
    gray_image = cv2.cvtColor(cv_image, cv2.COLOR_BGR2GRAY)
    cv2.imshow("Im", gray_image)
    cv2.waitKey(3)
    width = np.size(gray_image,1)
    height = np.size(gray_image,0)
    raw = gray_image.tostring()
    # initialize the Processor
    image = zbar.Image(width, height, 'Y800', raw)
    scanner.scan(image)
    global tags
    global locations
    
    global f
    #print robot_odometry.pose.pose.position.x
    for symbol in image:
      if not (symbol.data in tags):
	tags.append(symbol.data)
	(roll, pitch, yaw) = tf.transformations.euler_from_quaternion([robot_odometry.pose.pose.orientation.x, robot_odometry.pose.pose.orientation.y, robot_odometry.pose.pose.orientation.z, robot_odometry.pose.pose.orientation.w])
	locations.append(str(robot_odometry.pose.pose.position.x)+" "+str(robot_odometry.pose.pose.position.y)+" "+str(yaw))
	print tags,locations
    del(image)
  
  def Odometry_callback(self,msg):
    global robot_odometry
    robot_odometry = deepcopy(msg)



    
def shutdown_opretions():
  print("Shutting down")
  cv2.destroyAllWindows()
  i = 0
  for item in tags:
    f.write("%s " % item)
    f.write("%s\n" % locations[i])
    i = i+1
  f.close()
      
      
      
      
      
def main(args):
  rospy.init_node("zbar_komodo")
  ic = image_converter()
  rospy.on_shutdown(shutdown_opretions)
  try:
    rate = rospy.Rate(10)
    rospy.spin()
  except KeyboardInterrupt:
    print("Shutting down")
    cv2.destroyAllWindows()
    i = 1
    for item in tags:
      f.write("%s " % item)
      f.write("%s\n" % locations[i])
      i = i+1
    f.close()


if __name__ == '__main__':
    main(sys.argv)
