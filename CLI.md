# Arduino CLI

See https://github.com/arduino/arduino-cli

# Getting started with SimpleSerialPrinter sketch

List your boards:
```sh
arduino-cli board list
```
Example output, used in the following commands:
```
Port         Type              Board Name  FQBN            Core       
/dev/ttyACM0 Serial Port (USB) Arduino Uno arduino:avr:uno arduino:avr
```

Install the RadiationWatch library:
```sh
arduino-cli lib install RadiationWatch
```

Compile the SimpleSerialPrinter example (for the Arduino Uno platform):
```sh
arduino-cli compile --fqbn arduino:avr:uno examples/SimpleSerialPrinter
```

Upload the sketch to the board (to the Arduino Uno on serial port `/dev/ttyACM0`):
```sh
arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:avr:uno examples/SimpleSerialPrinter
```

Look at the serial output of the sketch (you may need to use the reset button of your Arduino board for the sketch to launch properly):
```sh
cat /dev/ttyACM0
```
Example output:
```
A wild gamma ray appeared
0.07 uSv/h +/- 0.07
A wild gamma ray appeared
0.13 uSv/h +/- 0.09
A wild gamma ray appeared
0.16 uSv/h +/- 0.09
A wild gamma ray appeared
0.09 uSv/h +/- 0.05
A wild gamma ray appeared
0.06 uSv/h +/- 0.03
A wild gamma ray appeared
0.07 uSv/h +/- 0.03
```

# Using interrupts with SerialPrinterEnableInterrupt sketch

Install the EnableInterrupt library, required for the sketch:
```sh
arduino-cli lib install EnableInterrupt
```

Compile the SerialPrinterEnableInterrupt example (for the Arduino Uno platform):
```sh
arduino-cli compile --fqbn arduino:avr:uno examples/SerialPrinterEnableInterrupt
```

Upload the sketch to the board (to the Arduino Uno on serial port `/dev/ttyACM0`):
```sh
arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:avr:uno examples/SerialPrinterEnableInterrupt
```

Look at the serial output of the sketch (you may need to use the reset button of your Arduino board for the sketch to launch properly):
```sh
cat /dev/ttyACM0
```
Example output:
```
A wild gamma ray appeared
0.08 uSv/h +/- 0.08
A wild gamma ray appeared
0.09 uSv/h +/- 0.06
Argh, noise, please stop moving
A wild gamma ray appeared
0.08 uSv/h +/- 0.05
A wild gamma ray appeared
0.07 uSv/h +/- 0.04
A wild gamma ray appeared
0.05 uSv/h +/- 0.02
A wild gamma ray appeared
0.05 uSv/h +/- 0.02
```
