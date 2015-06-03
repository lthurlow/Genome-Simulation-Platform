#!/usr/bin/python

import sys,os

if len(sys.argv) == 1:
  print "./strip.py file_to_strip.csv"
  exit(1)

filename = sys.argv[1]
if os.path.isfile(filename):
  f = open(filename,"r")
  print f  

  fn_out = filename.split(".")[0] + "_STRIP.csv"
  fout = open(fn_out, "w")

  for line in f:
    c = line.split(",")
    for word in c:
      if "bandwidth_used" in word:
        k = line.split(",")[1:]
        del k[-2]
        del k[1]
        if (k[-1].strip() is "0"):
          k[-1] = "1250"
        fout.writelines(str((','.join(k)).strip())+'\n')


else:
  print filename, " does not exist"
  exit(1)
