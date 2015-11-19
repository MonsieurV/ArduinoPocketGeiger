#include "Arduino.h"
#include "RadiationWatch.h"

int volatile radiationCount = 0;
int volatile noiseCount = 0;
bool volatile radiationFlag = false;
bool volatile noiseFlag = false;
// Message buffer for output.
char msg[256];

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
  _prevTime = 0;
  index = 0;
  _cpm = 0;
  cpmIndex = 0;
  cpmIndexPrev = 0;
  _duration = 0;
  totalSec = 0;
  totalHour = 0;
  cpmTimeMSec = 0;
  cpmTimeSec = 0;
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
  // Init time.
  _prevTime = millis();
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
  // TODO: do not use an index but measure time ellasped to determine when
  // to process the statistics.
  // TODO Update on radiation pulse, but not too fast (let a lag) so we does not
  // count radiation pulse when there are also noise. (use radiationFlag)
  if(index > 10000) {
    int currentTime = millis();
    // No noise detected in 10000 loops.
    if(noiseCount == 0) {
      // Shift an array for counting log for each 6 sec.
      if(totalSec % 6 == 0 && cpmIndexPrev != totalSec) {
        cpmIndexPrev = totalSec;
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
      // Get ready time for 10000 loops.
      cpmTimeMSec += abs(currentTime - _prevTime);
      // Transform from msec. to sec. (to prevent overflow).
      if(cpmTimeMSec >= 1000) {
        cpmTimeMSec -= 1000;
        // Add measurement time to calcurate cpm readings (max=20min.)
        if(cpmTimeSec >= 20*60)
          cpmTimeSec = 20*60;
        else
          cpmTimeSec++;
        // Total measurement time.
        totalSec++;
        //Transform from sec. to hour. (to prevent overflow).
        const int kSecondsInHour = 60 * 60;
        if(totalSec >= kSecondsInHour) {
          totalSec -= kSecondsInHour;
          totalHour++;
        }
      }
      index = 0;
    }
    // Initialization for next 10000 loops.
    _prevTime = currentTime;
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

char* RadiationWatch::printKey()
{
  // CSV-formatting for output.
  return "hour[h],sec[s],count,cpm,uSv/h,uSv/hError";
}

char* RadiationWatch::printStatus()
{
  //String buffers of float values for serial output
  char cpmBuff[20];
  char uSvBuff[20];
  char uSvdBuff[20];
  // Elapsed time of measurement (max=20min.)
  double min = cpmTime();
  dtostrf(cpm(), -1, 3, cpmBuff);
  dtostrf(uSvh(), -1, 3, uSvBuff);  // uSv/h
  dtostrf(uSvhError(), -1, 3, uSvdBuff);  // error of uSv/h
  // Format message.
  sprintf(msg, "%d,%d.%03d,%d,%s,%s,%s",
    totalHour,totalSec,
    cpmTimeMSec,
    radiationCount,
    cpmBuff,
    uSvBuff,
    uSvdBuff
    );
  return msg;
}

double RadiationWatch::cpmTime()
{
  return cpmTimeSec / 60.0;
}

double RadiationWatch::cpm()
{
  double min = cpmTime();
  return (min) ? _cpm / min : 0;
}

// cpm = uSv x alpha
static const double kAlpha = 53.032;

double RadiationWatch::uSvh()
{
  return cpm() / kAlpha;
}

double RadiationWatch::uSvhError()
{
  double min = cpmTime();
  return (min) ? sqrt(_cpm) / min / kAlpha : 0;
}
