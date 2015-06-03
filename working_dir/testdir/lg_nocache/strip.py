#!/usr/bin/python

import os

d = {}

for x in os.listdir:
  f = open(x)
  d[str(x)] = f[:-3]
print d 
