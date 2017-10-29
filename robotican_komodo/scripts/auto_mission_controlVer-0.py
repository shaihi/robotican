#!/usr/bin/env python
#auto move from points
#import fileinput
#import tf
#import math

import rospy
from actionlib_msgs.msg import GoalStatusArray
#from geometry_msgs.msg import Twist
#from move_base_msgs.msg import MoveBaseActionGoal
from nav_msgs.msg import Odometry
from move_base_msgs.msg import MoveBaseActionFeedback
from geometry_msgs.msg import PoseStamped
from globalClass import Globals

def odometrycallback(data):
    global g

    #print("Odometry trigerred !!!")
    g.quaternion = (data.pose.pose.orientation.x, data.pose.pose.orientation.y,  \
		    data.pose.pose.orientation.z,  \
		    data.pose.pose.orientation.w)
    #print("Odometry trigerred !!!{0}".format(g.quaternion))
    euler = tf.transformations.euler_from_quaternion(g.quaternion)
    #print("x={0}, y = {1}, z={2}-{3},{4},{5}".format(round(data.pose.pose.position.x, 2), \
    #round(data.pose.pose.position.y,2),round(data.pose.pose.position.z,2),  euler[0], euler[1], euler[ 2 ]))

def Twistcallback(data):
    rospy.loginfo("Twist Fired !!!")

def MoveRobotToPoint():
  print ("Moving To Point #{0}".format(g.currentPoint))
  publishTargetPoint()


def feedbackCallback(data):
    global g

    g.lastpos = (data.feedback.base_position.pose.position.x,  \
	  data.feedback.base_position.pose.position.y, \
	  data.feedback.base_position.pose.position.z, \
	  data.feedback.base_position.pose.orientation.x, \
	  data.feedback.base_position.pose.orientation.y, \
	  data.feedback.base_position.pose.orientation.z, \
	  data.feedback.base_position.pose.orientation.w )
    #print("Last Pos = {0}".format(g.lastpos))

def InitRospy():
    rospy.init_node('Mission_Control', anonymous=True)
    #pub = rospy.Publisher('/move_base/goal', MoveBaseActionGoal, queue_size=10)
    rospy.Subscriber("/odometry/filtered", Odometry, odometrycallback)
    rospy.Subscriber("/move_base/status", GoalStatusArray, statuscallback, queue_size=10)
    rospy.Subscriber("/move_base/feedback", MoveBaseActionFeedback, feedbackCallback)
    #rospy.Subscriber("/nav_vel", Twist, Twistcallback)
    g.pub = rospy.Publisher("/move_base_simple/goal", PoseStamped, queue_size=10)


def statuscallback(data):
    if len(data.status_list) == 0:
      return

    global g



    ind = len(data.status_list) - 1
    #print("Status={0},{1}- {2}".format(data.status_list[0].status,g.lastStatus,ind))
    if g.lastStatus != data.status_list[ind].status:
      g.lastStatus = data.status_list[ind].status

      if g.lastStatus == 1: #command accepted by the robot
	  print ("Command Received....#{0}".format(g.currentPoint))
	  g.RobotState = 1

      if g.lastStatus == 3 and g.RobotState == 1: #robot arrived to target after "confirming" the command
	  if g.currentPoint < len(g.list):
	    print ("Arrived To Point {0}".format(g.currentPoint))
	    g.setNextTargetPoint()
	  else:
	    g.RobotState = -1
	    print ("Home Sweet Home....")





      if g.lastStatus == 4 and g.RobotState == 1:  #targewt inaccecible
	  #print("Unable...\n")
	  print ("Can't arrive to Point {0}".format(g.currentPoint))
	  if g.currentPoint < len(g.list):
	    g.setNextTargetPoint()


def stateMachine():
   #print("Robot State={0}".format(g.RobotState))
   if (g.RobotState == 0):  #wait some time before setting the target
     g.TimeToPublish = g.TimeToPublish - 1
     if (g.TimeToPublish == 0):
       g.PublishNextPoint()

   #if (g.RobotState == 1):  #setting target accepted by the robot robot is in navigation mode



def main():
    global g
    print ('Start Node Mission_Control')
    g = Globals()
    InitRospy()

    r = rospy.Rate(1)    #hz
    while not rospy.is_shutdown() and g.RobotState <> -1:
      stateMachine()
      r.sleep()
    print("Job Done...")

if __name__=='__main__':
    main()