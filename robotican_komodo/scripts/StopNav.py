#!/usr/bin/env python

import rospy
from actionlib_msgs.msg import GoalStatusArray
#from geometry_msgs.msg import Twist
#from move_base_msgs.msg import MoveBaseActionGoal
from nav_msgs.msg import Odometry
from move_base_msgs.msg import MoveBaseActionFeedback
from geometry_msgs.msg import PoseStamped

from std_msgs.msg import Float32

from actionlib_msgs.msg import GoalID



def main():
    global g
    print ('Start Stop Navigation')

    rospy.init_node('Mission_Stop', anonymous=True)
    #pub = rospy.Publisher('/move_base/goal', MoveBaseActionGoal, queue_size=10)

    cancel_pub = rospy.Publisher('move_base/cancel', GoalID, queue_size=10)


    #setCancelState(True)
    goalId = GoalID()
    goalId.id = ''



    num = 0

    r = rospy.Rate(1)    #hz
    while not rospy.is_shutdown() and num < 3:
      goalId.stamp.secs = 0
      goalId.stamp.nsecs = 0
      num = num + 1

      print ("Canceling !!!!")
      cancel_pub.publish(goalId)
      r.sleep()

    print ('Done !!!')

if __name__=='__main__':
    main()
