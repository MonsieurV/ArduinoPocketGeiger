#include "RadiationWatch.h"
/*
This example works as the SimpleSerialPrinter, except it also generates
a tick noise for each radiation hitting the Pocket Geiger.

You need a piezo buzzer or similar connected to pin 8.
*/

const int PIN_TONE = 8;
RadiationWatch radiationWatch;

void onRadiation()
{
  // Output classic geiger counter tick noise.
  tone(PIN_TONE, 800, 1);
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
