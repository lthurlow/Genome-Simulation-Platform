#!/usr/bin/env python
import sys
import io
import operator
from collections import defaultdict

if (len(sys.argv) <= 1 or len(sys.argv) > 3):
  print "Invalid Argument: avg_links formatted-output-file platform-file"
  exit(1)

fo = open(sys.argv[1],'r')
pl = open(sys.argv[2],'r')

nodes = []
d = defaultdict(int)

for l in fo:
  if ("START" in l):
    x,y,z = l.partition('to')
    t = (x.split()[-1],z.split()[0])
    nodes.append(t)

for n in nodes:
  pl.seek(0, 0)
  x = "h%s" % n[0]
  y = "h%s" % n[1]
#  print n
  for l in pl:
    if (x in l and y in l):
      z = pl.next()
      a,b,c = z.partition('id')
      while ( b.isalnum()):
        k1 = c.strip("=\"l")[0].isdigit()
        k2 = c.strip("=\"l")[1].isdigit()
        if ( k1 is True and k2 is True):
          k3 = int(c.strip("=\"l")[0:2])
        else:
          k3 = int(c.strip("=\"l")[0])
#        print k3
        d[k3] += 1
        a,b,c = c.partition('id')

#mx = max(d.iteritems(), key=operator.itemgetter(1))[0]
#mn = max(d.iteritems(), key=operator.itemgetter(1))[-1]
mxv = 0
mxk = 0
mnv = 100000000000000
mnk = 0
tot = 0
for k in d:
  if d[k] > mxv:
    mxv = d[k]
    mxk = k
  if mnv > d[k]:
    mnv = d[k]
    mnk = k
  tot += d[k]

avg = tot/float(len(nodes))
print "Average # of links used: %f per request" % avg
print "Link used the most: %d => used %d times" % (mxk,mxv)
print "Link used the least: %d => used %d times" % (mnk,mnv)
