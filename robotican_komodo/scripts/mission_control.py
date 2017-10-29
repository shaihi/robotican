#!/usr/bin/env python
#111111

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
  def publishPoint(self):
    self.pub.publish(self.pnt)

  def readpos(self,filename):
      self.posfile = open(filename, "r")
      str = self.posfile.read()
      self.list = str.split("\n")
      self.posfile.close()

  def __init__(self):
    self.RobotState = 1
    self.lastStatus = -1;
    self.pub = rospy.Publisher("/move_base_simple/goal", PoseStamped, queue_size=10)
    self.quaternion = (0,0,0,0)
    self.lastpos = (99999,99999,99999,99999)
    self.readpos("/home/user/pos.txt")
    self.Num = 0
    #self.CommandStatus = 0
    self.pnt = PoseStamped()
    self.pnt.header.frame_id = 'map'


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

def PrintTargets():
  for i in xrange(0, len(g.list) - 1):
    print(str(i) + ") " + g.list[i])
  tar = input("Type the Target Number:" )
  publishTargetPoint(int(tar))


def toQuaternion(yaw):
    return [math.sin(yaw * 0.5), math.cos(yaw * 0.5)]

def statuscallback(data):
    global g
    if len(data.status_list) == 0:
      return
    g.RobotSglobal = 2
    #print("status fired !!! {0}".format())
    #rospy.loginfo(data.status_list[0].status)
    if g.lastStatus != data.status_list[0].status:
      g.lastStatus = data.status_list[0].status
      #if g.lastStatus == 1:  #message received
	#  g.CommandStatus = 1
	#  print("Command Receibved...\n")

      if g.lastStatus == 3: #robot arrived to target
	  #print("Arrived...\n")
	  g.RobotState = 2


      if g.lastStatus == 4:  #targewt inaccecible
	  #print("Unable...\n")
	  g.RobotState = 2

def feedbackCallback(data):
    global g

    #print("feed back trigerred {0} {1} !!!".format(data.status.status, data.feedback.base_position.pose.position.x))

    g.lastpos = (data.feedback.base_position.pose.position.x,  \
	  data.feedback.base_position.pose.position.y, \
	  data.feedback.base_position.pose.position.z, \
	  data.feedback.base_position.pose.orientation.x, \
	  data.feedback.base_position.pose.orientation.y, \
	  data.feedback.base_position.pose.orientation.z, \
	  data.feedback.base_position.pose.orientation.w )

def InitRospy():
    rospy.init_node('Mission_Control', anonymous=True)
    #pub = rospy.Publisher('/move_base/goal', MoveBaseActionGoal, queue_size=10)
    rospy.Subscriber("/odometry/filtered", Odometry, odometrycallback)
    rospy.Subscriber("/move_base/status", GoalStatusArray, statuscallback)
    rospy.Subscriber("/move_base/feedback", MoveBaseActionFeedback, feedbackCallback)
    #rospy.Subscriber("/nav_vel", Twist, Twistcallback)
    g.pub = rospy.Publisher("/move_base_simple/goal", PoseStamped, queue_size=10)


def publishTargetPoint(Indexpoint):
    global g
    g.Num = g.Num + 1
    print( "Publishing: {0},{1}".format(g.Num, Indexpoint))
    pt = g.list[Indexpoint].split(" ")

    numsf = [float(pt[0]), float(pt[1]), float(pt[2])]
    print ("Source = X = {0}, Y= {1}, Theta = {2}".format(numsf[0], numsf[1], numsf[2]))

    qt = toQuaternion(numsf[2] )
    #qt = tf.transformations.quaternion_from_euler(0, 0, numsf[2] )
    q = [numsf[0], numsf[1], qt[0],qt[1]]

    print ("Converted: X={0}, Y={1}, ox[0]={2}, oy[1]={3}, oz[3]={2}, ow[3]={5}\n".format(numsf[0], numsf[1], q[0], q[1],q[2],q[3]))

    g.pnt.pose.position.x = q[0]
    g.pnt.pose.position.y = q[1]
    g.pnt.pose.position.z = 0
    g.pnt.pose.orientation.x = 0
    g.pnt.pose.orientation.y = 0
    g.pnt.pose.orientation.z = q[2]
    g.pnt.pose.orientation.w = q[3]

    g.publishPoint()


def main():
    global g
    print ('Start Node Mission_Control')
    g = Globals()
    InitRospy()
    PrintTargets()
    r = rospy.Rate(1) # 1hz
    while not rospy.is_shutdown():
      print (1)
      if g.RobotState == 2:
	  print ("R=2")
	  PrintTargets()
	  g.RobotState = 3

      r.sleep()
      print (2)


if __name__=='__main__':
    main()
