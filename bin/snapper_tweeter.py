#!/usr/bin/python

import os, time

t = time.localtime()
month = t.tm_mon
day = t.tm_mday
hms = "%d:%d:%d" % (t.tm_hour, t.tm_min, t.tm_sec)
formatted_date = "%d-%d-%d-%s" % (t.tm_year, t.tm_mon, t.tm_mday, hms)
filename = formatted_date + ".jpg"
root = '/home/pi/images/';
path = root + "/" + filename

#os.system("touch /tmp/raspistill")
os.system("raspistill -w 1280 -h 1024 -o " + path)
#os.system("touch /tmp/t_update")
os.system("t update -f " + path)

