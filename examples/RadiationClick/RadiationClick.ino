//////////////////////////////////////////////////
// Radiation-Watch.org
// URL http://www.radiation-watch.org/
//////////////////////////////////////////////////

#include "RadiationWatch.h"

int PIN_TONE = 8;

RadiationWatchPrinter radiationWatch(2, 5, 0);

void onRadiationPulse() {
  // Output classic geiger counter tick noise
	tone(PIN_TONE, 800, 1);
	radiationWatch.printStatus();
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
  radiationWatch.printStatus();
}

void loop()
{
  radiationWatch.loop();
}


