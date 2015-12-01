#include "RadiationWatch.h"
/*
This simple example tell you on serial each time a gamma radiation hits the
Pocket Geiger and print the current micro Sievert dose. It also notify you of
the presence of vibration that prevent proper measurements.
*/

RadiationWatch radiationWatch;

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
