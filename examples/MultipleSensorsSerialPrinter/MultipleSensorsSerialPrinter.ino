/*
This example intends to show how to use the library with multiple Pocket Geiger
wired to the Arduino.

It is based on the SerialPrinterEnableInterrupt, as we need the EnableInterrupt library
for attaching interrupts to more than two pins (at least on Arduino Uno).

In the example, we assumes we have two Pocket Geiger connected, with the wiring described below.

First Pocket Geiger is wired to:
* SIG pin to Arduino pin 2
* NS pin to Arduino pin 3

Second Pocket Geiger is wired to:
* SIG pin to Arduino pin 4
* NS pin to Arduino pin 5

See a list of pins supported here:
https://github.com/GreyGnome/EnableInterrupt/wiki/Usage#pin--port-bestiary

You can install the EnableInterrupt library from the Arduino Library Manager:
http://www.arduinolibraries.info/libraries/enable-interrupt
*/

// EnableInterrupt from https://github.com/GreyGnome/EnableInterrupt
// include it before RadiationWatch.h
#include "EnableInterrupt.h"
#include "RadiationWatch.h"

// First Pocket Geiger: signPin = 2, noisePin = 3
RadiationWatch radiationWatch1(2, 3);
// Second Pocket Geiger: signPin = 4, noisePin = 5
RadiationWatch radiationWatch2(4, 5);
// Fairness counter.
unsigned int counter = 0;

void onRadiation1()
{
  Serial.println("[PG1] A wild gamma ray appeared");
  Serial.print("[PG1] ");
  Serial.print(radiationWatch1.uSvh());
  Serial.print(" uSv/h +/- ");
  Serial.println(radiationWatch1.uSvhError());
}

void onNoise1()
{
  Serial.println("[PG1] Argh, noise, please stop moving");
}

void onRadiation2()
{
  Serial.println("[PG2] A wild gamma ray appeared");
  Serial.print("[PG2] ");
  Serial.print(radiationWatch2.uSvh());
  Serial.print(" uSv/h +/- ");
  Serial.println(radiationWatch2.uSvhError());
}

void onNoise2()
{
  Serial.println("[PG2] Argh, noise, please stop moving");
}

void setup()
{
  Serial.begin(9600);
  radiationWatch1.setup();
  radiationWatch2.setup();
  // Register the callbacks.
  radiationWatch1.registerRadiationCallback(&onRadiation1);
  radiationWatch1.registerNoiseCallback(&onNoise1);
  radiationWatch2.registerRadiationCallback(&onRadiation2);
  radiationWatch2.registerNoiseCallback(&onNoise2);
}

void loop()
{
  // Just to be sure both processing loops get
  // equal amount of processing. Should not be useful/has
  // any effect, except maybe at high radiation count levels.
  if (counter % 2 == 0) {
    radiationWatch1.loop();
    radiationWatch2.loop();
  } else {
    radiationWatch2.loop();
    radiationWatch1.loop();
  }
  counter++;
}
