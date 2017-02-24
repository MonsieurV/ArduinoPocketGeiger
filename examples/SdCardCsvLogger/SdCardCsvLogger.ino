 // You can change here the history length to reduce the memory footprint.
#define HISTORY_LENGTH 200
#include "RadiationWatch.h"
#include <SD.h>
#include <SPI.h>
/*
Same as SerialCsvLogger, but store the CSV values to an SD card.
*/

RadiationWatch radiationWatch;
const byte sdCardPin = 4;
File dataFile;
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
  // For each radiation write the current values to the SD card in CSV.
  csvStatus(radiationWatch, dataFile);
  dataFile.flush();
}

void setup()
{
  Serial.begin(9600);
  // SD setup.
  if(!SD.begin(sdCardPin)) {
    Serial.println("SD card init failed, or not present");
    return;
  }
  dataFile = SD.open("rad.csv", FILE_WRITE);
  if(!dataFile) {
    Serial.println("Failed to open file");
    return;
  }
  radiationWatch.setup();
  // Print the CSV keys and the initial values.
  csvKeys(dataFile);
  csvStatus(radiationWatch, dataFile);
  dataFile.flush();
  // Register the callback.
  radiationWatch.registerRadiationCallback(&onRadiationPulse);
}

void loop()
{
  radiationWatch.loop();
}
