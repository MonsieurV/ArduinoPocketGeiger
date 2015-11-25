#include "RadiationWatch.h"
#include <SD.h>
#include <SPI.h>

RadiationWatch radiationWatch(2, 3, 0, 1);
const int sdCardPin = 4;
File dataFile;
char* str;

void onRadiationPulse() {
  // For each radiation update the values sent to serial and SD card in CSV.
  str = radiationWatch.csvStatus();
  Serial.println(str);
  dataFile.println(str);
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
  dataFile = SD.open("radiation.csv", FILE_WRITE);
  if(!dataFile) {
    Serial.println("Failed to open file");
    return;
  }
  // Setup the lib and register the callback.
  radiationWatch.setup();
  radiationWatch.registerRadiationCallback(&onRadiationPulse);
  // Print the CSV keys and the initial values.
  str = radiationWatch.csvKeys();
  Serial.println(str);
  dataFile.println(str);
  str = radiationWatch.csvStatus();
  Serial.println(str);
  dataFile.println(str);
}

void loop()
{
  radiationWatch.loop();
}
