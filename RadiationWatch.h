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
    RadiationWatch(int signPin, int noisePin, int signIRQ);
    
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
  
    static const unsigned int kHistoryCount = 50; // was 200

    double _cpmHistory[kHistoryCount]; //History of count rates
    
    int _prevTime;

    int _signPin;   //Radiation Pulse (Yellow)
    int _noisePin;  //Vibration Noise Pulse (White)
	int _signIRQ;	//The IRQ number for the radiation pulse pin (depends on Arduino model)

    int index; //Number of loops
    
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
    RadiationWatchPrinter(int signPin, int noisePin, int signIRQ);
  
    virtual void printKey();
    virtual void printStatus();
};

#endif

