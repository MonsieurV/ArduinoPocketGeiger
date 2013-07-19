#include <RadiationWatch.h>

RadiationWatch radiationWatch(2, 5);

void setup()
{
  //Serial setup
  //9600bps
  Serial.begin(9600);
  
  radiationWatch.setup();

  Serial.println("starting...");
}

unsigned int i = 0;

void loop()
{
  radiationWatch.loop();
  
  if (radiationWatch.isAvailable() && i >= 1000) {
    Serial.print("cpm = ");
    Serial.println(radiationWatch.cpm());
    
    i = 0;
  }
  
  i++;
}

