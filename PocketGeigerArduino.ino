//////////////////////////////////////////////////
// Radiation-Watch.org
// URL http://www.radiation-watch.org/
//////////////////////////////////////////////////

#include "RadiationWatch.h"

RadiationWatch radiationWatch(2, 5);

void setup()
{
  //Serial setup
  //9600bps
  Serial.begin(9600);
  
  radiationWatch.setup();

  radiationWatch.printKey();
}

void loop()
{
  radiationWatch.loop();
}


