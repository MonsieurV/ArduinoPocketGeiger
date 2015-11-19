#include "RadiationWatch.h"

RadiationWatch radiationWatch(2, 3, 0, 1);

void onRadiation()
{
  Serial.println("A wild gamma ray appeared");
}

void onNoise()
{
  Serial.println("Argh, noise, please stop moving");
}

void periodicStatusPrinter()
{
  Serial.println(radiationWatch.csvStatus());
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
