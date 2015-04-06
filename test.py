#!/usr/bin/python

import ao

dev = ao.AudioDevice('wav', filename='foo.wav', bits=16)
f = open('test.wav', 'r')
data = f.read()
dev.play(data, len(data))

