#include "RadiationWatch.h"

RadiationWatch radiationWatch(2, 3, 0, 1);

void onRadiation() {
  Serial.println("Gamma ray");
}

void onNoise() {
  Serial.println("Noise");
}

void periodicStatusPrinter() {
  Serial.println(radiationWatch.printStatus());
}

void setup()
{
  //Serial setup
  //9600bps
  Serial.begin(9600);

  radiationWatch.setup();

  // Register the callbacks.
  radiationWatch.registerRadiationCallback(&onRadiation);
  radiationWatch.registerNoiseCallback(&onNoise);

  Serial.println(radiationWatch.printKey());
  Serial.println(radiationWatch.printStatus());
}

void loop()
{
  radiationWatch.loop();
}
