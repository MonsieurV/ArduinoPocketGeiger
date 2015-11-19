#include "RadiationWatch.h"

const int PIN_TONE = 8;
RadiationWatch radiationWatch(2, 3, 0, 1);

void onRadiation()
{
  // Output classic geiger counter tick noise.
  tone(PIN_TONE, 800, 1);
  Serial.println(radiationWatch.csvStatus());
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
  Serial.println(radiationWatch.csvKeys());
  Serial.println(radiationWatch.csvStatus());
}

void loop()
{
  radiationWatch.loop();
}
