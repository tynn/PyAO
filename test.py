#!/usr/bin/python

import ao

print "Default driver id:", ao.default_driver_id()
print "Bigendian?", ao.is_big_endian()
dev = ao.AudioDevice(ao.default_driver_id(), bits=16)
f = open('test.wav', 'r')
data = f.read()
dev.play(data, len(data))

