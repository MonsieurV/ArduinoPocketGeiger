//////////////////////////////////////////////////
// Radiation-Watch.org
// URL http://www.radiation-watch.org/
//////////////////////////////////////////////////

#ifndef RadiationWatch_h
#define RadiationWatch_h

class RadiationWatch
{
  public:
    RadiationWatch(int signPin, int noisePin);
    
    void setup();
    void loop();
    
    void printKey();
  
    int signPin();
    int noisePin();
    
    void printStatus();
  
  private:
    static const unsigned int kHistoryCount = 200;

    double _cpmHistory[kHistoryCount]; //History of count rates
    
    int _prevTime;

    int _signPin;   //Radiation Pulse (Yellow)
    int _noisePin;  //Vibration Noise Pulse (White)

    static const double alpha = 53.032; // cpm = uSv x alpha
    
    int index; //Number of loops
    
    int signCount;  //Counter for Radiation Pulse
    int noiseCount;  //Counter for Noise Pulse
    
    int sON;//Lock flag for Radiation Pulse
    int nON;//Lock flag for Noise Puls
    
    double cpm; //Count rate [cpm] of current
    int cpmIndex;//Position of current count rate on cpmHistory[]
    int cpmIndexPrev;//Flag to prevent duplicative counting
    
    int totalSec; //Elapsed time of measurement [sec]
    int totalHour; //Elapsed time of measurement [hour]
    
    //Time settings for CPM calcuaration
    int cpmTimeMSec;
    int cpmTimeSec;
};

#endif

