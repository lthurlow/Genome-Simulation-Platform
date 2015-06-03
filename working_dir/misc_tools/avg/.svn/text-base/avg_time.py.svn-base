#!/usr/bin/env python
import os
import io
import sys
import operator

if (len(sys.argv) <= 1 or len(sys.argv) > 2):
  print "Invalid Argument: avg_time simgrid-output-file"
  exit(1)

a = open(sys.argv[1],'r')

d1 = {}

for l in a:
  a, b, key = l.partition('ID:')
  val = float(a.split()[1][:-1])
  key = int(key.strip())
  
  if key in d1:
    old_val = d1[key]
    val = val - old_val
    val = round(val,4)
    d1[key] = val
  else:
    d1[key] = val


sort = sorted(d1.iteritems(), key=operator.itemgetter(0))
avg = 0
for k in sort:
  print "Genome %d took %f secs" % (k[0],k[1])
  avg += k[1]

avg = avg /len(d1)
print "Average time was %f" % avg
