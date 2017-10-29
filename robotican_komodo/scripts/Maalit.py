#!/usr/bin/env python

import math
import numpy
import rospy
from std_msgs.msg import Float64
from sensor_msgs.msg import JointState


class Globals:
  def __init__(self):
    self.targetHeight = 0.0
    self.joint = JointState()
    self.arrived = False
    self.delta = 1
    self.currentHeight = 0.0
    #self.pub = rospy.Publisher("/joint_states", JointState, queue_size=10)
    self.SendJoin = rospy.Publisher("/send_joint_states", Float64, queue_size=10)

def InitRospy():
    global g
    rospy.init_node('Elevator_Control', anonymous=True)
    rospy.Subscriber("/get_target_position", Float64, getmaalitcallback)


def getmaalitcallback(data):
  global g

  g.targetHeight = data.data

  g.arrived = True

  print ("Got Elevator Target Height {0} !!!".format(g.targetHeight))

  if g.targetHeight > g.currentHeight:
    g.delta = 1
  else:
    g.delta = -1



def main():
    global g
    InitRospy()

    num = Float64()

    g = Globals()

    r = rospy.Rate(1)    #hz

    while not rospy.is_shutdown():
      #print("Step3 Tra = {0} current={1}".format(g.targetHeight,g.currentHeight))
      if abs(g.targetHeight - g.currentHeight) > 1:
	g.currentHeight = g.currentHeight + g.delta
	num.data = g.currentHeight
	g.SendJoin.publish(num)
      else:
	if g.arrived == True:
	  g.currentHeight = g.currentHeight + g.delta
	  num.data = g.currentHeight
	  g.SendJoin.publish(num)
	  g.arrived = False
      r.sleep()

if __name__=='__main__':
    main()


