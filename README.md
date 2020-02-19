# Arduino Pocket Geiger library

An Arduino library to interface with the [Radiation Watch Pocket Geiger counter](http://www.radiation-watch.org/) (Type 5).

![](/misc/type5.jpg?raw=true "Radiation Watch Type 5 Pocket Geiger counter")

The library monitors the Pocket Geiger through interrupts, processes the hourly [Sievert dose](https://en.wikipedia.org/wiki/Sievert) and allows to get back the data using a simple interface. It comes with examples to print data to Serial or log it on a SD card, giving you some ideas of what to do with it.

Learn more about the Pocket Geiger counter on the Radiation Watch [FAQ](http://www.radiation-watch.co.uk/faqs) or on [our blog](https://blog.ytotech.com/2015/12/06/radiation-watch-arduino/). Actually it is not a proper Geiger-Müller counter, but a Photodiode PIN sensor that can nevertheless effectively counts Gamma radiation.

## Getting started

### Install the library

Go the Arduino Library Manager and search for [RadiationWatch](http://arduinolibraries.info/libraries/radiation-watch): install, that's it!

Alternatively download [the library as a zip](https://github.com/MonsieurV/ArduinoPocketGeiger/archive/master.zip), extract it and put it in your Arduino `libraries` folder. (see [instructions](https://www.arduino.cc/en/Guide/Libraries))

If you link the command-line, check [`CLI.md`](/CLI.md) for [`arduino-cli`](https://github.com/arduino/arduino-cli) installation and getting started instructions.

### Wire your Pocket Geiger board to your Arduino

First connect your Pochet Geiger Type 5 board to your Arduino. By default, the binding used is as follow:

| Pocket Geiger pin | Arduino pin | Standing for |
| ----------------- | ----------- | ------------ |
| `+V` | `5V` | Alimentation pin (DC 3V~9V) |
| `GND` | `GND` | Ground pin |
| `SIG` | `2` | Radiation-detection pulse pin |
| `NS` | `3` | Noise-detection pulse pin |

![](/misc/wiring.png?raw=true "Radiation Watch and Arduino wiring")

You can change the pins used by the library at its initialization. Remember to use pins that [enable external interrupts](https://www.arduino.cc/en/Reference/AttachInterrupt).

```
/*
signPin is the radiation pulse pin, which must match the signIrq number.
noisePin is the noise pulse pin, which must match the noiseIrq number.
*/
RadiationWatch radiationWatch(signPin, noisePin);
```

[Pocket Geiger Type 5 interface specification](http://www.radiation-watch.co.uk/uploads/5t.pdf).

Look at the [buildlogs folder](/buildlogs/README.md) for more step by step instructions.

### Launch the Serial printer example

Go to your Arduino examples and launch the `RadiationWatch` -> `SimpleSerialPrinter` sketch. It outputs to the serial port the current emission level for each  [gamma ray](https://en.wikipedia.org/wiki/Gamma_ray) that hits the Pocket Geiger counter.

Your done! Enjoy your -hopefully- low exposure to Gamma radiation.

![](/misc/snapshot_get_starter_example.png?raw=true "Serial output for the example")

### Plot in real-time with Python

Load the `RadiationWatch` -> `SerialCsvLogger` sketch to your Arduino. Then launch the Python script [`serial_plot.py`](/examples/SerialCsvLogger/serial_plot.py).

You need Python installed with [matplotlib](http://matplotlib.org/) and [pyserial](https://github.com/pyserial/pyserial). You may need to set the serial port corresponding to your Arduino in the Python script.

The script will output the radiation level in real-time.

![](/misc/real_time_plotting.gif?raw=true "Real-time plotting")

### Log the results to an SD card

You'll need the [Arduino Ethernet shield](https://www.arduino.cc/en/Main/ArduinoEthernetShield) (recommended) or the [SparkFun microSD shield](https://www.sparkfun.com/products/12761).

Load the `RadiationWatch` -> `SdCardCsvLogger` sketch to your Arduino. The radiation measurements will be stored in a `rad.csv` file on the microSD card, so you can then retrieve and process them later.

Remember however the Pocket Geiger can't record correctly in presence of vibration, so try to keep the whole stationary during the measurements. For an advanced mobile unit of measurement you may look at the [bGeigie Nano](http://blog.safecast.org/bgeigie-nano/) from the Safecast project.

-----------------------

## Misc.

Find this cool? Check out [our blog](http://blog.ytotech.com) or [send us a note](mailto:yoan@ytotech.com).

This lib was [featured on the Radiation Watch website](http://www.radiation-watch.org/2015/12/type5arduino-new-and-open-sourced.html).

If you search a building idea, the [Playspoon fully integrated LCD Geiger counter](http://www.playspoon.com/wiki/index.php/GeigerCounter) may inspire you.

Happy hacking!

### Credits

Created upon the [thomasaw](https://github.com/thomasaw/RadiationWatch) and [Tourmal](https://github.com/Toumal/RadiationWatch) libraries, themselves based on the [Radiation Watch sample code](http://radiation-watch.sakuraweb.com/share/ARDUINO.zip).

[@puuu](https://github.com/puuu) [contributed](https://github.com/MonsieurV/ArduinoPocketGeiger/graphs/contributors) some major enhancements to the library structure (see #4 and #5).

### Contribute

Feel free to [open a new ticket](https://github.com/MonsieurV/ArduinoPocketGeiger/issues/new) or submit a PR to improve the lib.
