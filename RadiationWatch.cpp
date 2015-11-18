//////////////////////////////////////////////////
// Radiation-Watch.org
// URL http://www.radiation-watch.org/
//////////////////////////////////////////////////

#include "Arduino.h"
#include "RadiationWatch.h"

RadiationWatch* context;
// Message buffer for serial output.
char msg[256];

// Function used to forward the call to the member function
// RadiationWatch::onRadiationPulse when a radiation pulse is detected.
// (The attachInterrupt() can't take a member function as parameter)
// See. http://stackoverflow.com/questions/12662891/c-passing-member-function-as-argument
void onRadiationPulseForwarder() {
  context->onRadiationPulse();
}

//Called via Interrupt when a radiation pulse is detected
void RadiationWatch::onRadiationPulse() {
  // Serial.println(micros());
  signCount++;
  // if(_radiationPulseCallback != NULL) {
  //   // Trigger the registered callback.
  //   _radiationPulseCallback();
  // }
  // Serial.println(micros());
}

RadiationWatch::RadiationWatch(int signPin, int noisePin, int signIRQ) : _signPin(signPin), _noisePin(noisePin), _signIRQ(signIRQ)
{
  _prevTime = 0;
  index = 0;

  signCount = 0;
  noiseCount = 0;

  sON = 0;
  nON = 0;

  _cpm = 0;
  cpmIndex = 0;
  cpmIndexPrev = 0;

  totalSec = 0;
  totalHour = 0;

  cpmTimeMSec = 0;
  cpmTimeSec = 0;

  _radiationPulseCallback = NULL;
  context = this;
}

void RadiationWatch::setup()
{
  //PIN setting for Radiation Pulse
  pinMode(_signPin,INPUT);
  digitalWrite(_signPin,HIGH);

  //PIN setting for Noise Pulse
  pinMode(_noisePin,INPUT);
  digitalWrite(_noisePin,HIGH);

  //Attach interrupt handler to catch incoming radiation pulses,
  //and execute triggerRadiationPulse() when this happens.
  attachInterrupt(_signIRQ, onRadiationPulseForwarder, RISING);

  //Initialize cpmHistory[]
  for(int i = 0; i < kHistoryCount;i++ )
  {
    _cpmHistory[i] = 0;
  }

  _prevTime = millis();
}

void RadiationWatch::registerRPCallback(void (*callback)(void)) {
  _radiationPulseCallback = callback;
}

int RadiationWatch::signPin()
{
  return digitalRead(_signPin);
}

int RadiationWatch::noisePin()
{
  return digitalRead(_noisePin);
}

unsigned long beginAt = 0;
unsigned long endAt = 0;

void RadiationWatch::loop()
{
  // Raw data of Noise Pulse: Not-detected -> Low, Detected -> High
  int noise = noisePin();

  //Noise Pulse normally keeps high for about 100[usec]
  if(noise==1 && nON==0)
  {//Deactivate Noise Pulse counting for a while
    nON = 1;
    noiseCount++;
  }else if(noise==0 && nON==1){
    nON = 0;
  }

  //Output readings to serial port, after 10000 loops
  if(index==10000) //About 160-170 msec in Arduino Nano(ATmega328)
  {
    //Get current time
    int currTime = millis();

    if(noiseCount > 0) {
      Serial.println("Noise!");
      Serial.println(noiseCount);
    }
    //No noise detected in 10000 loops
    if(noiseCount == 0)
    {
      //Shift an array for counting log for each 6 sec.
      if( totalSec % 6 == 0 && cpmIndexPrev != totalSec)
      {
        cpmIndexPrev = totalSec;
        cpmIndex++;

        if(cpmIndex >= kHistoryCount)
        {
          cpmIndex = 0;
        }

        if(_cpmHistory[cpmIndex] > 0)
        {
          _cpm -= _cpmHistory[cpmIndex];
        }
        _cpmHistory[cpmIndex] = 0;
      }

      noInterrupts();

      //Store count log
      _cpmHistory[cpmIndex] += signCount;
      //Add number of counts
      _cpm += signCount;

      //Get ready time for 10000 loops
      cpmTimeMSec += abs(currTime - _prevTime);
      //Transform from msec. to sec. (to prevent overflow)
      if(cpmTimeMSec >= 1000)
      {
        cpmTimeMSec -= 1000;
        //Add measurement time to calcurate cpm readings (max=20min.)
        if( cpmTimeSec >= 20*60 )
        {
          cpmTimeSec = 20*60;
        }else{
          cpmTimeSec++;
        }

        //Total measurement time
        totalSec++;
        //Transform from sec. to hour. (to prevent overflow)
        const int kSecondsInHour = 60 * 60;
        if(totalSec >= kSecondsInHour)
        {
          totalSec -= kSecondsInHour;
          totalHour++;
        }
      }

      index=0;
    }


    //Initialization for next 10000 loops
    _prevTime = currTime;
    signCount = 0;
    noiseCount = 0;
    interrupts();

    Serial.println(printStatus());
  }

  index++;
}

char* RadiationWatch::printKey() {}

char* RadiationWatch::printStatus() {}

boolean RadiationWatch::isAvailable()
{
  return cpmTime() != 0;
}

double RadiationWatch::cpmTime()
{
  return cpmTimeSec / 60.0;
}

double RadiationWatch::cpm()
{
  double min = cpmTime();

  if (min != 0) {
    return _cpm / min;
  }
  else {
    return 0;
  }
}

static const double kAlpha = 53.032; // cpm = uSv x alpha

double RadiationWatch::uSvh()
{
  return cpm() / kAlpha;
}

double RadiationWatch::uSvhError()
{
  double min = cpmTime();

  if (min != 0) {
    return sqrt(_cpm) / min / kAlpha;
  }
  else {
    return 0;
  }
}

RadiationWatchPrinter::RadiationWatchPrinter(int signPin, int noisePin, int signIRQ) : RadiationWatch(signPin, noisePin, signIRQ)
{
}

char* RadiationWatchPrinter::printKey()
{
  // CSV-formatting for output.
  return "hour[h],sec[s],count,cpm,uSv/h,uSv/hError";
}

char* RadiationWatchPrinter::printStatus()
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
    signCount,
    cpmBuff,
    uSvBuff,
    uSvdBuff
    );
  return msg;
}
