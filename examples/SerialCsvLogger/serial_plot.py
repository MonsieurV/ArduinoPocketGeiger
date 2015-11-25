#!/usr/bin/env python
# -*- coding: utf-8 -*-
""" Plot in real-time the gamma radiation level being logged to the serial
port. Because it's way more fun than reading a terminal.

You will need Python with MatPlotLib installed on your box.

Configure the serial port to which the Arduino is connected. By default,
we select the first port, but that may not be the good one."""
import serial
import matplotlib.pyplot as plt

# Edit the port, specifying either a name, or a number.
# port = '/dev/ttyACM0'
port = 0

fig = plt.figure()
axes = fig.add_subplot(111)
x = []
y = []
li, = axes.plot(x, y)
plt.ion()
plt.show()

with serial.Serial(port, 9600) as ser:
    print('Listening to %s' % (ser.name))
    while 1:
        line = ser.readline()[:-1]
        print(line)
        (time, count, cpm, uSvh, uSvhError) = line.split(',')
        if time == 'time(ms)':
            continue
        x.append(int(time))
        y.append(float(uSvh))
        li.set_data(x, y)
        axes.relim()
        axes.autoscale_view(True,True,True)
        plt.draw()
