#!/usr/bin/env python
import numpy as np
import rospy
import roslib
import tf
from sys import argv
import sys
from copy import deepcopy
import time

# Messages
from nav_msgs.msg import Odometry
from geometry_msgs.msg import Point, Quaternion, PoseWithCovarianceStamped

#Globals
amcl_position = PoseWithCovarianceStamped()
odometry_position = PoseWithCovarianceStamped()


          
class particle_filter_maintainer:
  def __init__(self): 
    self.odometry_sub = rospy.Subscriber("/mobile_base_controller/odom", PoseWithCovarianceStamped, self.Odometry_callback)
    self.amcl_sub = rospy.Subscriber("/amcl_pose", PoseWithCovarianceStamped, self.amcl_callback)
    PF_odom_pub = rospy.Publisher('/initialpose', PoseWithCovarianceStamped,queue_size=10)
    frame_id = '/odom'
    child_frame_id = '/base_footprint'
  def Odometry_callback(self,msg):
    global odometry_position
    odometry_position = deepcopy(msg)  
    
  def amcl_callback(self,msg):
    global amcl_position
    amcl_position = deepcopy(msg) 
    P_odom = np.array(odometry_position.pose.covariance)
    P_odom = P_odom.reshape(6,6)
    (r, p, y) = tf.transformations.euler_from_quaternion([odometry_position.pose.pose.orientation.x, odometry_position.pose.pose.orientation.y, odometry_position.pose.pose.orientation.z, odometry_position.pose.pose.orientation.w])
    X_odom = np.matrix('%s; %s; %s; %s; %s; %s' % (odometry_position.pose.pose.position.x, odometry_position.pose.pose.position.y, odometry_position.pose.pose.position.z, r,p,y)) 
    P_amcl = np.array(amcl_position.pose.covariance)
    P_amcl = P_amcl.reshape(6,6)
    print np.trace(P_amcl)
    #time.sleep(0.5)
    #(r, p, y) = tf.transformations.euler_from_quaternion([amcl_position.pose.pose.orientation.x, amcl_position.pose.pose.orientation.y, amcl_position.pose.pose.orientation.z, amcl_position.pose.pose.orientation.w])
    #X_amcl = np.matrix('%s; %s; %s; %s; %s; %s' % (amcl_position.pose.pose.position.x, amcl_position.pose.pose.position.y, amcl_position.pose.pose.position.z, r,p,y))
    #res = X_amcl - X_odom
    #S = P_odom + P_amcl
    #K = P_odom*np.linalg.inv(S)
    #X_odom = X_odom + K*res 
    #P_odom = P_odom - K*S*K.conj().transpose()
    #print X_odom
    #if np.trace(P_amcl) > 1:
      #pub_msg = PoseWithCovarianceStamped()
      #pub_msg.pose.covariance = tuple(P_odom.ravel().tolist())
      #pub_msg.pose.pose.orientation = Quaternion(*(kdl.Rotation.RPY(X_odom[3], X_odom[4], X_odom[5]).GetQuaternion()))
      #pub_msg.pose.pose.position = Point(X_odom[0], X_odom[1], X_odom[2])
      #pub_msg.header.stamp = rospy.Time.now()
      #pub_msg.header.frame_id = self.frame_id # i.e. '/odom'
      #pub_msg.child_frame_id = self.child_frame_id # i.e. '/base_footprint'
      #self.PF_odom_pub.publish(pub_msg)
      
     
def main(args):
  rospy.init_node("PF_maintainer")
  PFM = particle_filter_maintainer()
  try:
    rate = rospy.Rate(10)
    rospy.spin()
  except KeyboardInterrupt:
    print("Shutting down")
   


if __name__ == '__main__':
    main(sys.argv)

