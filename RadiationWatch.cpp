/*
 * Radiation Watch Pocket Geiger Type 5 library for Arduino.
 *
 * Documentation and usage at:
 * https://github.com/MonsieurV/RadiationWatch
 *
 * Contributed by:
 * Radiation Watch <http://www.radiation-watch.org/>
 * thomasaw <https://github.com/thomasaw>
 * Tourmal <https://github.com/Toumal>
 * Yoan Tournade <yoan@ytotech.com>
 */
#include "RadiationWatch.h"

byte volatile radiationCount = 0;
int volatile noiseCount = 0;
bool volatile radiationFlag = false;
bool volatile noiseFlag = false;
// Message buffer for output.
char _msg[60];

void _onRadiationHandler()
{
  radiationCount++;
  radiationFlag = true;
}

void _onNoiseHandler()
{
  noiseCount++;
  noiseFlag = true;
}

RadiationWatch::RadiationWatch(
  byte signPin, byte noisePin, byte signIrq, byte noiseIrq):
    _signPin(signPin), _noisePin(noisePin), _signIrq(signIrq),
    _noiseIrq(noiseIrq)
{
  previousTime = 0;
  _cpm = 0;
  cpmIndex = 0;
  cpmIndexPrev = 0;
  totalTime = 0;
  _radiationCallback = NULL;
  _noiseCallback = NULL;
}

void RadiationWatch::setup()
{
  pinMode(_signPin, INPUT);
  digitalWrite(_signPin, HIGH);
  pinMode(_noisePin, INPUT);
  digitalWrite(_noisePin, HIGH);
  // Initialize cpmHistory[].
  for(int i = 0; i < HISTORY_LENGTH; i++)
    _cpmHistory[i] = 0;
  // Init measurement time.
  previousTime = millis();
  // Attach interrupt handlers.
  attachInterrupt(_signIrq, _onRadiationHandler, RISING);
  attachInterrupt(_noiseIrq, _onNoiseHandler, RISING);
}

unsigned long loopTime = 0;
unsigned int loopElasped = 0;

void RadiationWatch::loop()
{
  // Process radiation dose if the process period has elapsed.
  loopElasped = loopElasped + abs(millis() - loopTime);
  loopTime = millis();
  if(loopElasped > PROCESS_PERIOD) {
    unsigned long currentTime = millis();
    if(noiseCount == 0) {
      // Shift an array for counting log for each 6 seconds.
      // TODO Will overflox after 18 hours of measurement.
      unsigned int totalTimeSec = totalTime / 1000;
      if(totalTimeSec % HISTORY_UNIT == 0 && cpmIndexPrev != totalTimeSec) {
        cpmIndexPrev = totalTimeSec;
        cpmIndex++;
        if(cpmIndex >= HISTORY_LENGTH)
          cpmIndex = 0;
        if(_cpm > 0 && _cpmHistory[cpmIndex] > 0) {
          _cpm -= _cpmHistory[cpmIndex];
          Serial.println("Decrement _cpm from history");
          Serial.println(_cpmHistory[cpmIndex]);
          Serial.println(_cpm);
        }
        _cpmHistory[cpmIndex] = 0;
      }
      noInterrupts();
      // Store count log.
      _cpmHistory[cpmIndex] += radiationCount;
      // Add number of counts.
      _cpm += radiationCount;
      // Get the elapsed time.
      totalTime += abs(currentTime - previousTime);
      loopElasped = 0;
    }
    // Initialization for next N loops.
    previousTime = currentTime;
    radiationCount = 0;
    noiseCount = 0;
    interrupts();
    // Enable the callbacks.
    if(_radiationCallback && radiationFlag) {
      radiationFlag = false;
      _radiationCallback();
    }
    if(_noiseCallback && noiseFlag) {
      noiseFlag = false;
      _noiseCallback();
    }
  }
}

void RadiationWatch::registerRadiationCallback(void (*callback)(void))
{
  _radiationCallback = callback;
}

void RadiationWatch::registerNoiseCallback(void (*callback)(void))
{
  _noiseCallback = callback;
}

char* RadiationWatch::csvKeys()
{
  // CSV-formatting for output.
  return "time(ms),count,cpm,uSv/h,uSv/hError";
}

char* RadiationWatch::csvStatus()
{
  // Format message. We use dtostrf() to format float to string.
  char cpmBuff[8];
  char uSvBuff[8];
  char uSvdBuff[8];
  dtostrf(cpm(), -1, 3, cpmBuff);
  dtostrf(uSvh(), -1, 3, uSvBuff);
  dtostrf(uSvhError(), -1, 3, uSvdBuff);
  sprintf(_msg, "%lu,%d,%s,%s,%s",
    totalTime, radiationCount, cpmBuff, uSvBuff, uSvdBuff);
  return _msg;
}

unsigned long RadiationWatch::duration()
{
  return totalTime;
}

float RadiationWatch::cpm()
{
  // cpm = uSv x alpha
  float min = cpmTime();
  return (min > 0) ? _cpm / min : 0;
}

static const float kAlpha = 53.032;

float RadiationWatch::uSvh()
{
  return cpm() / kAlpha;
}

float RadiationWatch::uSvhError()
{
  float min = cpmTime();
  return (min > 0) ? sqrt(_cpm) / min / kAlpha : 0;
}
