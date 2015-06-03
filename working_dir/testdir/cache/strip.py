#!/usr/bin/python

import os

d = {}

for x in os.listdir('.'):
  f = open(x).readlines()
  d[str(x)] = f[-3:]

for x in d:
  print x
  for y in d[x]:
    print y
