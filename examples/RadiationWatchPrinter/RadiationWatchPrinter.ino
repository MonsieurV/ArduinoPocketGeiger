//////////////////////////////////////////////////
// Radiation-Watch.org
// URL http://www.radiation-watch.org/
//////////////////////////////////////////////////

#include "RadiationWatch.h"

RadiationWatchPrinter radiationWatch(2, 5, 0);

void onRadiationPulse() {
	Serial.println("Pulse!");
}

void setup()
{
  //Serial setup
  //9600bps
  Serial.begin(9600);
  
  radiationWatch.setup();

  // Register the callback.
  radiationWatch.registerRPCallback(&onRadiationPulse);

  radiationWatch.printKey();
}

void loop()
{
  radiationWatch.loop();
}


