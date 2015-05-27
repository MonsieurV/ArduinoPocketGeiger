# Radiation Watch Arduino library

An Arduino library to interface with the [Radiation Watch](http://www.radiation-watch.org/) Pocket Geiger

Based on [thomasaw's library](https://github.com/thomasaw/RadiationWatch) and the [Radiation Watch sample code](http://radiation-watch.sakuraweb.com/share/ARDUINO.zip)

Changes in this version:

* Changed from using loops for the radiation detection to interrupts

* Allow to declare a custom event handler for when a radiation pulse is detected
