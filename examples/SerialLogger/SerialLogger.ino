//////////////////////////////////////////////////
// Radiation-Watch.org
// URL http://www.radiation-watch.org/
//////////////////////////////////////////////////

#include "RadiationWatch.h"

RadiationWatch radiationWatch(2, 3, 0, 1);

void onRadiationPulse() {
  Serial.println(radiationWatch.printStatus());
}

void setup()
{
  //Serial setup
  //9600bps
  Serial.begin(9600);

  radiationWatch.setup();

  // Register the callback.
  radiationWatch.registerRPCallback(&onRadiationPulse);

  Serial.println(radiationWatch.printKey());
  Serial.println(radiationWatch.printStatus());
}

void loop()
{
  radiationWatch.loop();
}
