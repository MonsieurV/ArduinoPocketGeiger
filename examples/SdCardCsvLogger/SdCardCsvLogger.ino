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

void onRadiationPulse() {
  // For each radiation write the current values to the SD card in CSV.
  dataFile.println(radiationWatch.csvStatus());
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
  dataFile.println(radiationWatch.csvKeys());
  dataFile.println(radiationWatch.csvStatus());
  dataFile.flush();
  // Register the callback.
  radiationWatch.registerRadiationCallback(&onRadiationPulse);
}

void loop()
{
  radiationWatch.loop();
}
