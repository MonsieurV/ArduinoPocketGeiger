#!/usr/bin/env python
# -*- coding: utf-8 -*-
""" Plot in real-time the gamma radiation level being logged to the serial
port. Because it's way more fun than reading a terminal.

You will need Python with the matplotlib and pyserial packages installed on
your box.

Configure the serial port to which the Arduino is connected. By default,
we select the first port, but that may not be the good one."""
import serial
import matplotlib.pyplot as plt
import sys

# Edit the port, specifying either a name, or a number.
port = 0
# port = '/dev/ttyACM0'
if len(sys.argv) > 1:
    port = sys.argv[1]

fig = plt.figure()
axes = fig.add_subplot(111)
times = []
bottomError = []
radiation = []
topError = []
bottomLine, = axes.plot(times, bottomError, color='green', label=u'Error (-)')
radiationLine, = axes.plot(times, radiation, color='blue', label=u'Radiation dose')
topLine, = axes.plot(times, topError, color='green', label=u'Error (+)')
plt.ion()
plt.legend(loc='best')
plt.xlabel(u'Time (min)')
plt.ylabel(u'Dose (μSv/h)')
plt.title(u'Gamma rays (γ)')
plt.show()

with serial.Serial(port, 9600) as ser:
    print('Listening to %s' % (ser.name))
    while 1:
        line = ser.readline()[:-1].decode()
        print(line)
        (time, count, cpm, uSvh, uSvhError) = line.split(',')
        if time == 'time(ms)':
            continue
        times.append(float(time) / 1000 / 60)
        bottomError.append(float(uSvh) - float(uSvhError))
        radiation.append(float(uSvh))
        topError.append(float(uSvh) + float(uSvhError))
        bottomLine.set_data(times, bottomError)
        radiationLine.set_data(times, radiation)
        topLine.set_data(times, topError)
        axes.relim()
        axes.autoscale_view(True,True,True)
        plt.draw()
        fig.canvas.flush_events()
