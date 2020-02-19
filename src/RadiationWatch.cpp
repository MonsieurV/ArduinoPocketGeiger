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
#define LIBCALL_RADIATIONWATCH
#include "RadiationWatch.h"

int volatile RadiationWatch::_radiationCount = 0;
int volatile RadiationWatch::_noiseCount = 0;

#ifdef ESP8266
// For ESP8266 boards,
// (https://arduino.stackexchange.com/questions/34496/how-to-have-compiler-know-whether-arduino-or-esp8266)
// we add ICACHE_RAM_ATTR to move the interrupt handler to RAM.
// (Instead of flash)
// See:
// https://arduino-esp8266.readthedocs.io/en/latest/faq/a02-my-esp-crashes.html#other-causes-for-crashes
// https://hackaday.io/project/28220-one-tube-nixie-clock/log/71448-progress (Interrupt Handling)
void ICACHE_RAM_ATTR RadiationWatch::_onRadiationHandler()
{
  _radiationCount++;
}

void ICACHE_RAM_ATTR RadiationWatch::_onNoiseHandler()
{
  _noiseCount++;
}
#else
void RadiationWatch::_onRadiationHandler()
{
  _radiationCount++;
}

void RadiationWatch::_onNoiseHandler()
{
  _noiseCount++;
}
#endif

RadiationWatch::RadiationWatch(byte signPin, byte noisePin):
    _signPin(signPin), _noisePin(noisePin)
{
  previousTime = 0;
  previousHistoryTime = 0;
  _count = 0;
  historyIndex = 0;
  historyLength = 0;
  _radiationCallback = NULL;
  _noiseCallback = NULL;
}

void RadiationWatch::setup()
{
  // Enable the ~20kΩ pull-up resistor for radiation and noise
  // pins.
  // For pullup resistors on Arduino, see https://www.arduino.cc/en/Tutorial/DigitalPins
  pinMode(_signPin, INPUT_PULLUP);
  // The noise signal is low by default (no noise), so we could install a pull-down resistor.
  // However this is not available on Arduino platform, so we goes with a pull-up resistor.
  // As the PocketGeiger board pull down the noise signal to low if working, this is not an issue.
  pinMode(_noisePin, INPUT_PULLUP);
  // Initialize _countHistory[].
  for(int i = 0; i < HISTORY_LENGTH; i++)
    _countHistory[i] = 0;
  _count = 0;
  historyIndex = 0;
  historyLength = 0;
  // Init measurement time.
  previousTime = millis();
  previousHistoryTime = millis();
  // Attach interrupt handlers.
  setupInterrupt();
}

void RadiationWatch::loop()
{
  // Process radiation dose if the process period has elapsed.
  unsigned long currentTime = millis();
  if(currentTime - previousTime >= PROCESS_PERIOD) {
    noInterrupts();
    int currentCount = _radiationCount;
    int currentNoiseCount = _noiseCount;
    _radiationCount = 0;
    _noiseCount = 0;
    interrupts();
    if(currentNoiseCount == 0) {
      // Store count log.
      _countHistory[historyIndex] += currentCount;
      // Add number of counts.
      _count += currentCount;
    }
    // Shift an array for counting log for each 6 seconds.
    if(currentTime - previousHistoryTime >= HISTORY_UNIT * 1000) {
      previousHistoryTime += (unsigned long)(HISTORY_UNIT * 1000);
      historyIndex = (historyIndex + 1) % HISTORY_LENGTH;
      if(historyLength < (HISTORY_LENGTH-1)) {
        // Since, we overwrite the oldest value in the history,
        // the effective maximum length is HISTORY_LENGTH-1
        historyLength++;
      }
      _count -= _countHistory[historyIndex];
      _countHistory[historyIndex] = 0;
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

unsigned long RadiationWatch::integrationTime()
{
  return (historyLength * HISTORY_UNIT * 1000UL
          + previousTime - previousHistoryTime);
}

int RadiationWatch::currentRadiationCount() {
  noInterrupts();
  int currentCount = _radiationCount;
  interrupts();
  return currentCount;
}

unsigned long RadiationWatch::radiationCount() {
  return _count;
}

double RadiationWatch::cpm()
{
  // cpm = uSv x alpha
  double min = integrationTime() / 60000.0;
  return (min > 0) ? radiationCount() / min : 0;
}

double RadiationWatch::uSvh()
{
  return cpm() / kAlpha;
}

double RadiationWatch::uSvhError()
{
  double min = integrationTime() / 60000.0;
  return (min > 0) ? sqrt(radiationCount()) / min / kAlpha : 0;
}
