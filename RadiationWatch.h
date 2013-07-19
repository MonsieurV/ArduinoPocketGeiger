//////////////////////////////////////////////////
// Radiation-Watch.org
// URL http://www.radiation-watch.org/
//////////////////////////////////////////////////

#ifndef RadiationWatch_h
#define RadiationWatch_h

#include "Arduino.h"

class RadiationWatch
{
  public:
    RadiationWatch(int signPin, int noisePin);
    
    void setup();
    void loop();
    
    double cpm();
    double uSvh();       // uSv/h
    double uSvhError();  // error of uSv/h
    
    boolean isAvailable();
    
    virtual void printKey();
    virtual void printStatus();
  
  protected:
    int signPin();
    int noisePin();
    
    double cpmTime();
  
    static const unsigned int kHistoryCount = 200;

    double _cpmHistory[kHistoryCount]; //History of count rates
    
    int _prevTime;

    int _signPin;   //Radiation Pulse (Yellow)
    int _noisePin;  //Vibration Noise Pulse (White)

    int index; //Number of loops
    
    int signCount;  //Counter for Radiation Pulse
    int noiseCount;  //Counter for Noise Pulse
    
    int sON;//Lock flag for Radiation Pulse
    int nON;//Lock flag for Noise Puls
    
    double _cpm; //Count rate [cpm] of current
    int cpmIndex;//Position of current count rate on cpmHistory[]
    int cpmIndexPrev;//Flag to prevent duplicative counting
    
    int totalSec; //Elapsed time of measurement [sec]
    int totalHour; //Elapsed time of measurement [hour]
    
    //Time settings for CPM calcuaration
    int cpmTimeMSec;
    int cpmTimeSec;
};

class RadiationWatchPrinter : public RadiationWatch
{
  public:
    RadiationWatchPrinter(int signPin, int noisePin);
  
    virtual void printKey();
    virtual void printStatus();
};

#endif

