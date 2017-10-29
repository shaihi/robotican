#!/usr/bin/python
from sys import argv
import rospy
import sys
import roslib
import cv2
from std_msgs.msg import Bool
from sensor_msgs.msg import Image
from cv_bridge import CvBridge, CvBridgeError
#globals:
pictures_counter = 0
class image_converter:

  

  def __init__(self): 
    image_raw_topic = "/usb_cam/image_raw".format(rospy.get_name())
    self.bridge = CvBridge()
    
    self.command_bool_sub = rospy.Subscriber("/take_picture", Bool, self.command_bool_callback)
     
    
    # create a Processor
    
    
  def image_callback(self,data):
    try:
      cv_image = self.bridge.imgmsg_to_cv2(data, "bgr8")
    except CvBridgeError as e:
      print(e)
    global pictures_counter
    pictures_counter = pictures_counter + 1
    picName = "robot_pic_"+str(pictures_counter)+".jpg"
    cv2.imwrite(picName, cv_image)
    self.image_sub.unregister()
  
  def command_bool_callback(self,msg):
    if msg.data == 1:
      self.image_sub = rospy.Subscriber("/usb_cam/image_raw",Image,self.image_callback)



    
def shutdown_opretions():
  print("Shutting down")
  cv2.destroyAllWindows()
  
      
      
      
      
      
def main(args):
  rospy.init_node("image_saver_komodo")
  ic = image_converter()
  rospy.on_shutdown(shutdown_opretions)
  try:
    rate = rospy.Rate(10)
    rospy.spin()
  except KeyboardInterrupt:
    print("Shutting down")
    cv2.destroyAllWindows()
    


if __name__ == '__main__':
    main(sys.argv)
