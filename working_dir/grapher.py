#!/usr/bin/python

import os, subprocess

for k in xrange(0,5):
  for x in xrange(10,55,5):
    for y in xrange(100,1100,100):
      Z = "./sim -H -p "+ str(y) + " -c " + str(x) + " -C " + str(x) +" -v 20 > testdir/" + str(x) + "-" + str(y) + "-" + str(k) + ".out"
      os.system(Z)
