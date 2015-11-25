#include "RadiationWatch.h"

RadiationWatch radiationWatch(2, 3, 0, 1);

void onRadiationPulse() {
  // For each radiation update the values sent to serial in CSV.
  Serial.println(radiationWatch.csvStatus());
}

void setup()
{
  Serial.begin(9600);
  radiationWatch.setup();
  // Register the callback.
  radiationWatch.registerRadiationCallback(&onRadiationPulse);
  // Print the CSV keys and the initial values.
  Serial.println(radiationWatch.csvKeys());
  Serial.println(radiationWatch.csvStatus());
}

void loop()
{
  radiationWatch.loop();
}
