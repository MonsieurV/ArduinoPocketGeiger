#include "RadiationWatch.h"
#include <SD.h>

const int chipSelect = 4;
RadiationWatchPrinter radiationWatch(2, 5, 0);
File dataFile;
char* str;

void onRadiationPulse() {
	Serial.println("Pulse!");
	str = radiationWatch.printStatus();
  Serial.println(str);
	dataFile.println(str);
}

void setup()
{
	// Serial init.
  Serial.begin(9600);
	// SD setup.
	pinMode(10, OUTPUT);
	if(!SD.begin(chipSelect)) {
		Serial.println("SD card init failed, or not present");
		return;
	}
	dataFile = SD.open("log.txt", FILE_WRITE);
	if(!dataFile) {
		Serial.println("Failed to open file");
		return;
	}
  // Setup the lib and register the pulse callback.
  radiationWatch.setup();
  radiationWatch.registerRPCallback(&onRadiationPulse);
	// Print first status on serial and on SD card.
	str = radiationWatch.printKey();
  Serial.println(str);
	dataFile.println(str);
	str = radiationWatch.printStatus();
  Serial.println(str);
	dataFile.println(str);
}

void loop()
{
  radiationWatch.loop();
}
