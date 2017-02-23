/*
This example is basically the same as SimpleSerialPrinter but utilize
the EnableInterrupt library.  EnableInterrupt make pin change
interrupts available and thus allows to use arbitrary pins.
*/

// EnableInterrupt from https://github.com/GreyGnome/EnableInterrupt
// include it before RadiationWatch.h
#include "EnableInterrupt.h"
#include "RadiationWatch.h"

// Use any pin as you like
// Here: signPin = 5, noisePin = 6
RadiationWatch radiationWatch(5, 6);

void onRadiation()
{
  Serial.println("A wild gamma ray appeared");
  Serial.print(radiationWatch.uSvh());
  Serial.print(" uSv/h +/- ");
  Serial.println(radiationWatch.uSvhError());
}

void onNoise()
{
  Serial.println("Argh, noise, please stop moving");
}

void setup()
{
  Serial.begin(9600);
  radiationWatch.setup();
  // Register the callbacks.
  radiationWatch.registerRadiationCallback(&onRadiation);
  radiationWatch.registerNoiseCallback(&onNoise);
}

void loop()
{
  radiationWatch.loop();
}
