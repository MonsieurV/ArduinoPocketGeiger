/*
 * Radiation Watch Pocket Geiger Type 5 library for Arduino.
 *
 * Documentation and usage at:
 * https://github.com/MonsieurV/RadiationWatch
 *
 * Released under MIT License. See LICENSE file.
 *
 * Contributed by:
 * Radiation Watch <http://www.radiation-watch.org/>
 * thomasaw <https://github.com/thomasaw>
 * Tourmal <https://github.com/Toumal>
 * Yoan Tournade <yoan@ytotech.com>
 */
#include "RadiationWatch.h"
#if !defined(ARDUINO_ARCH_AVR) && !defined(ESP8266)
#include <avr/dtostrf.h>
#endif

int volatile radiationCount = 0;
int volatile noiseCount = 0;
// Message buffer for output.
char _msg[60];

void _onRadiationHandler()
{
  radiationCount++;
}

void _onNoiseHandler()
{
  noiseCount++;
}

RadiationWatch::RadiationWatch(byte signPin, byte noisePin):
    _signPin(signPin), _noisePin(noisePin)
{
  previousTime = 0;
  previousHistoryTime = 0;
  _cpm = 0;
  cpmIndex = 0;
  totalTime = 0;
  _radiationCallback = NULL;
  _noiseCallback = NULL;
}

void RadiationWatch::setup()
{
  pinMode(_signPin, INPUT_PULLUP);
  pinMode(_noisePin, INPUT_PULLUP);
  // Initialize cpmHistory[].
  for(int i = 0; i < HISTORY_LENGTH; i++)
    _cpmHistory[i] = 0;
  // Init measurement time.
  previousTime = millis();
  previousHistoryTime = millis();
  // Attach interrupt handlers.
  attachInterrupt(digitalPinToInterrupt(_signPin), _onRadiationHandler, FALLING);
  attachInterrupt(digitalPinToInterrupt(_noisePin), _onNoiseHandler, RISING);
}

void RadiationWatch::loop()
{
  // Process radiation dose if the process period has elapsed.
  unsigned long currentTime = millis();
  if(currentTime - previousTime >= PROCESS_PERIOD) {
    noInterrupts();
    int currentCount = radiationCount;
    int currentNoiseCount = noiseCount;
    radiationCount = 0;
    noiseCount = 0;
    interrupts();
    if(currentNoiseCount == 0) {
      // Shift an array for counting log for each 6 seconds.
      if(currentTime - previousHistoryTime >= HISTORY_UNIT * 1000) {
        previousHistoryTime += (unsigned long)(HISTORY_UNIT * 1000);
        cpmIndex = (cpmIndex + 1) % HISTORY_LENGTH;
        _cpm -= _cpmHistory[cpmIndex];
        _cpmHistory[cpmIndex] = 0;
      }
      // Store count log.
      _cpmHistory[cpmIndex] += currentCount;
      // Add number of counts.
      _cpm += currentCount;
      // Get the elapsed time.
      totalTime += (currentTime - previousTime);
    }
    // Save time of current process period
    previousTime = currentTime;
    // Enable the callbacks.
    if(_noiseCallback && currentNoiseCount > 0) {
      _noiseCallback();
    }
    if(_radiationCallback && currentCount > 0) {
      _radiationCallback();
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
    totalTime, currentRadiationCount(), cpmBuff, uSvBuff, uSvdBuff);
  return _msg;
}

unsigned long RadiationWatch::duration()
{
  return totalTime;
}

int RadiationWatch::currentRadiationCount() {
  noInterrupts();
  int currentCount = radiationCount;
  interrupts();
  return currentCount;
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
