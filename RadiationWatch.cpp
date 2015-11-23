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
#include "Arduino.h"
#include "RadiationWatch.h"

int volatile radiationCount = 0;
int volatile noiseCount = 0;
bool volatile radiationFlag = false;
bool volatile noiseFlag = false;
// Message buffer for output.
char _msg[256];

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
  int signPin, int noisePin, int signIrq, int noiseIrq):
    _signPin(signPin), _noisePin(noisePin), _signIrq(signIrq),
    _noiseIrq(noiseIrq)
{
  previousTime = 0;
  index = 0;
  _cpm = 0;
  cpmIndex = 0;
  cpmIndexPrev = 0;
  totalTime = 0;
  cpmTime = 0;
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
  for(int i = 0; i < kHistoryCount; i++)
    _cpmHistory[i] = 0;
  // Init measurement time.
  previousTime = millis();
  // Attach interrupt handlers.
  attachInterrupt(_signIrq, _onRadiationHandler, RISING);
  attachInterrupt(_noiseIrq, _onNoiseHandler, RISING);
}

void RadiationWatch::registerRadiationCallback(void (*callback)(void))
{
  _radiationCallback = callback;
}

void RadiationWatch::registerNoiseCallback(void (*callback)(void))
{
  _noiseCallback = callback;
}

void RadiationWatch::loop()
{
  // Process radiation dose.
  // About 160-170 msec in Arduino Nano(ATmega328).
  // TODO: do not use an index but measure time elapsed to determine when
  // to process the statistics.
  // TODO Update on radiation pulse, but not too fast (let a lag) so we does not
  // count radiation pulse when there are also noise. (use radiationFlag)
  if(index > 10000) {
    unsigned long currentTime = millis();
    // No noise detected in 10000 loops.
    if(noiseCount == 0) {
      // Shift an array for counting log for each 6 seconds.
      int totalTimeSec = (int) totalTime / 1000;
      if(totalTimeSec % 6 == 0 && cpmIndexPrev != totalTimeSec) {
        cpmIndexPrev = totalTimeSec;
        cpmIndex++;
        if(cpmIndex >= kHistoryCount)
          cpmIndex = 0;
        if(_cpmHistory[cpmIndex] > 0)
          _cpm -= _cpmHistory[cpmIndex];
        _cpmHistory[cpmIndex] = 0;
      }
      noInterrupts();
      // Store count log.
      _cpmHistory[cpmIndex] += radiationCount;
      // Add number of counts.
      _cpm += radiationCount;
      // Get the elapsed time.
      totalTime += abs(currentTime - previousTime);
      cpmTime = totalTime;
      // CPM time is limited to 20 minutes (20*60*1000).
      // TODO Use a define
      if(cpmTime >= 1200000L)
        cpmTime = 1200000L;
      index = 0;
    }
    // Initialization for next 10000 loops.
    previousTime = currentTime;
    radiationCount = 0;
    noiseCount = 0;
    interrupts();
  }
  index++;
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

char* RadiationWatch::csvKeys()
{
  // CSV-formatting for output.
  return "time(ms),count,cpm,uSv/h,uSv/hError";
}

char* RadiationWatch::csvStatus()
{
  // Format message. We must use dtostrf() to format float to string.
  // String buffers of float values for output.
  char cpmBuff[10];
  char uSvBuff[10];
  char uSvdBuff[10];
  dtostrf(cpm(), -1, 3, cpmBuff);
  dtostrf(uSvh(), -1, 3, uSvBuff);
  dtostrf(uSvhError(), -1, 3, uSvdBuff);
  sprintf(_msg, "%lu,%d,%s,%s,%s",
    totalTime, radiationCount, cpmBuff, uSvBuff, uSvdBuff);
  return _msg;
}

double RadiationWatch::cpmTimeMin()
{
  // TODO Transform to a macro
  return ((long) cpmTime / 1000) / 60.0;
}

double RadiationWatch::cpm()
{
  double min = cpmTimeMin();
  Serial.println(min);
  return (min > 0) ? _cpm / min : 0;
}

// cpm = uSv x alpha
static const double kAlpha = 53.032;

double RadiationWatch::uSvh()
{
  return cpm() / kAlpha;
}

double RadiationWatch::uSvhError()
{
  double min = cpmTimeMin();
  return (min > 0) ? sqrt(_cpm) / min / kAlpha : 0;
}
