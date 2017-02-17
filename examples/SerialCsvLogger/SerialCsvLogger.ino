#include "RadiationWatch.h"
/*
Output the measurements on serial as CSV values.

You can use the joined Python script serial_plot.py to plot dynamically
the radiation level.
*/

RadiationWatch radiationWatch;
unsigned long csvStartTime;

void csvKeys(Print &print=Serial)
{
  // inizialise start time
  csvStartTime = millis();
  // Write CSV keys to a print class (default print class is Serial).
  print.println("time(ms),count,cpm,uSv/h,uSv/hError");
}

void csvStatus(RadiationWatch radiationWatch, Print &print=Serial)
{
  // Write CSV to a print class (default print class is Serial).
  // time(ms)
  print.print(millis() - csvStartTime);
  print.print(',');
  // count
  print.print(radiationWatch.radiationCount());
  print.print(',');
  // cpm
  print.print(radiationWatch.cpm(), 3);
  print.print(',');
  // uSv/h
  print.print(radiationWatch.uSvh(), 3);
  print.print(',');
  // uSv/hError
  print.print(radiationWatch.uSvhError(), 3);
  print.println("");
}

void onRadiationPulse() {
  // For each radiation update the values sent to serial in CSV.
  csvStatus(radiationWatch);
}

void setup()
{
  Serial.begin(9600);
  radiationWatch.setup();
  // Register the callback.
  radiationWatch.registerRadiationCallback(&onRadiationPulse);
  // Print the CSV keys and the initial values.
  csvKeys();
  csvStatus(radiationWatch);
}

void loop()
{
  radiationWatch.loop();
}
