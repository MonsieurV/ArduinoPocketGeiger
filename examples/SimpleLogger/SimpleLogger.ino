#include <RadiationWatch.h>

RadiationWatch radiationWatch(2, 3, 0);

void onRadiationPulse() {
  Serial.print("cpm = ");
  Serial.println(radiationWatch.cpm());
}

void setup()
{
  //Serial setup
  //9600bps
  Serial.begin(9600);

  radiationWatch.setup();

  // Register the callback.
  radiationWatch.registerRPCallback(&onRadiationPulse);

  Serial.println("starting...");
}

void loop()
{
  radiationWatch.loop();
}
