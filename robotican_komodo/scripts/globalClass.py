#!/usr/bin/env python
#auto move from points
import fileinput
import tf
import math

import rospy
from actionlib_msgs.msg import GoalStatusArray
#from geometry_msgs.msg import Twist
#from move_base_msgs.msg import MoveBaseActionGoal
from nav_msgs.msg import Odometry
from move_base_msgs.msg import MoveBaseActionFeedback
from geometry_msgs.msg import PoseStamped

class Globals:

  def readpos(self,filename):
      self.posfile = open(filename, "r")
      str = self.posfile.read()
      self.list = str.split("\n")
      self.posfile.close()
      self.list = self.list[0:len(self.list)-1]

  def __init__(self):
    self.lastStatus = -1;
    self.pnt = PoseStamped()

    self.pnt.header.frame_id = 'map'

    self.pub = rospy.Publisher("/move_base_simple/goal", PoseStamped, queue_size=10)
    self.readpos("/home/user/pos.txt")
    self.lastpos = []
    self.currentPoint = 0
    self.TimeToPublish = 2
    self.lastStatus = -1
    self.RobotState = 0

  def toQuaternion(self,yaw):
      return [math.sin(yaw * 0.5), math.cos(yaw * 0.5)]


  def publishTargetPoint(self):
      pt = self.list[self.currentPoint].split(" ")

      print ("Publishing Point {0}-x={1} y={2}".format(self.currentPoint, round(float(pt[0]), 2), round(float(pt[1]), 2)))

      qt = self.toQuaternion(float(pt[2]))

      self.pnt.pose.position.x = float(pt[0])
      self.pnt.pose.position.y = float(pt[1])
      self.pnt.pose.position.z = 0
      self.pnt.pose.orientation.x = 0
      self.pnt.pose.orientation.y = 0
      self.pnt.pose.orientation.z = qt[0]
      self.pnt.pose.orientation.w = qt[1]

      self.pub.publish(self.pnt)

  def gohome(self):
      print("Going Home !!!")
      self.pnt.pose.position.x = 0
      self.pnt.pose.position.y = 0
      self.pnt.pose.position.z = 0
      self.pnt.pose.orientation.x = 0
      self.pnt.pose.orientation.y = 0
      self.pnt.pose.orientation.z = 0
      self.pnt.pose.orientation.w = 1
      self.pub.publish(self.pnt)


  def PublishNextPoint(self):
    self.TimeToPublish = 20
    if (self.currentPoint < len(self.list)):
      self.publishTargetPoint()
    else:
      self.gohome()

  def setNextTargetPoint(self):
    self.RobotState = 0
    self.TimeToPublish = 3
    self.currentPoint = self.currentPoint + 1
