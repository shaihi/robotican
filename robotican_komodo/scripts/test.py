#!/usr/bin/env python

import fileinput
import tf
import math


global list

def readpos(filename):
    global list
    posfile = open(filename, "r")
    str = posfile.read()
    list = str.split("\n")
    posfile.close()


def toQuaternion(yaw):
    #cy = cos(yaw * 0.5)
    #sy = sin(yaw * 0.5)
    #cr = 1 #cos(roll * 0.5)
    #sr = 0 #sin(roll * 0.5)
    #cp = 1 #cos(pitch * 0.5)
    #sp = 0 #sin(pitch * 0.5)

    #w = cy #cy * cr * cp + sy * sr * sp;
    ##x = 0   #cy * sr * cp - sy * cr * sp;
    ##y = 0  #cy * cr * sp + sy * sr * cp;
    #z = sy  #sy * cr * cp - cy * sr * sp;
    return [math.sin(yaw * 0.5), math.cos(yaw * 0.5)]




def PrintTargets():
  global list
  print ("Number of Lines = {0}".format(len(list)))
  for i in xrange(0, len(list)-1):
    #print(str(i) + ") " + list[i])
    nums = list[ i ].split(" ")
    numsf = [float(nums[0]), float(nums[1]), float(nums[2])]
    print ("Source = X = {0}, Y= {1}, Theta = {2}".format(numsf[0], numsf[1], numsf[2]))
    euler = [numsf[2], 0, 0]
    #qt = tf.transformations.quaternion_from_euler(0, 0, numsf[2] )
    #q = [numsf[0], numsf[1], 0, qt[0], qt[1],qt[2],qt[3]]
    qun = toQuaternion(numsf[2])

    print ("Converted = X = {0}, Y= {1}, ox=0, oy=0, oz={2}, ow={3}\n".format(numsf[0], numsf[1], qun[0], qun[1]))



def main():
    global list
    readpos("/home/user/pos.txt")
    PrintTargets()



if __name__=='__main__':
    main()
