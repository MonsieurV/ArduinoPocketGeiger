# Arduino Radiation Watch library

An Arduino library to interface with the [Radiation Watch](http://www.radiation-watch.co.uk/) Pocket Geiger counter.  Type 5 .

The library monitors the Pocket Geiger through interrupts, processes the Sievert dose and allow to get back the data using a simple interface. It provides examples to print data to Serial, log it on a SD card or through internet, giving you some ideas of what to do with it.

## Getting started

### Install the library

Download [the library as a zip](https://github.com/MonsieurV/RadiationWatch/archive/master.zip), extract it and put it in your Arduino `libraries` folder. (see [instructions](https://www.arduino.cc/en/Guide/Libraries))

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
RadiationWatchPrinter radiationWatch(signPin, noisePin, signIrq, noiseIrq);
```

[Pocket Geiger Type 5 interface specification](http://www.radiation-watch.co.uk/uploads/5t.pdf).

### Launch the Serial printer example

Go to your Arduino examples and launch the `RadiationWatch` -> `RadiationWatchPrinter` sketch.

TODO Screenshot of the Serial output

## Usage

TODO Usage of the RadiationWatch class

-----------------------

Find this cool? Check out [our blog](http://blog.ytotech.com) and [send us a note](mailto:yoan@ytotech.com). Happy hacking!

### Credits

Created upon the [thomasaw](https://github.com/thomasaw/RadiationWatch) and [Tourmal](https://github.com/Toumal/RadiationWatch) libraries, themselves based on the [Radiation Watch sample code](http://radiation-watch.sakuraweb.com/share/ARDUINO.zip).

### Contribute

Feel free to [open a new ticket](https://github.com/MonsieurV/RadiationWatch/issues/new) or submit a PR to improve the lib.
