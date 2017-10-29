#!/usr/bin/env python

import rospy
from std_msgs.msg import Float64
from sensor_msgs.msg import JointState


class Globals:
  def __init__(self):
    self.targetHeight = 0.0
    self.currentHeight = 0.0
    self.delta = 0.1
    self.ismoving = False
    self.pub = rospy.Publisher("/get_target_position", Float64, queue_size=10)


def InitRospy():
    global g
    rospy.init_node('Elevator_Control', anonymous=True)
    #rospy.Subscriber("/testMaalitBenny", JointState, statusmaalitcallback)
    rospy.Subscriber("/send_joint_states", Float64, statusmaalitcallback)


def statusmaalitcallback(data):
  global g
  if data.data == g.targetHeight:
    g.ismoving = False
    print ("arrived !!!")



def main():
    global g
    InitRospy()

    g = Globals()

    r = rospy.Rate(1)    #hz
    while not rospy.is_shutdown():
      if not g.ismoving:
	g.ismoving = True
	pos = input("Enter Elevator Position: ")
	g.targetHeight = float(pos)
	g.pub.publish(g.targetHeight)

      r.sleep()

if __name__=='__main__':
    main()


