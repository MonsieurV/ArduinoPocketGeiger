//////////////////////////////////////////////////
// Radiation-Watch.org
// URL http://www.radiation-watch.org/
//////////////////////////////////////////////////

const double alpha=53.032; // cpm = uSv x alpha

int index=0; //Number of loops
char msg[256]=""; //Message buffer for serial output

int signCount=0;  //Counter for Radiation Pulse
int noiseCount=0;  //Counter for Noise Pulse

int sON=0;//Lock flag for Radiation Pulse
int nON=0;//Lock flag for Noise Puls

double cpm = 0; //Count rate [cpm] of current
int cpmIndex=0;//Position of current count rate on cpmHistory[]
int cpmIndexPrev=0;//Flag to prevent duplicative counting

//Timing Settings for Loop Interval
int currTime=0; 

int totalSec=0; //Elapsed time of measurement [sec]
int totalHour=0; //Elapsed time of measurement [hour]

//Time settings for CPM calcuaration
int cpmTimeMSec=0;
int cpmTimeSec=0;
int cpmTimeMin=0;

//String buffers of float values for serial output
char cpmBuff[20];
char uSvBuff[20];
char uSvdBuff[20];

class RadiationWatch
{
  public:
    RadiationWatch(int signPin, int noisePin);
    
    void setup();
    void loop();
    
    void printKey();
  
    int signPin();
    int noisePin();
  
    static const unsigned int kHistoryCount = 200;

    double _cpmHistory[kHistoryCount]; //History of count rates
    
    int _prevTime;

  private:
    int _signPin;   //Radiation Pulse (Yellow)
    int _noisePin;  //Vibration Noise Pulse (White)
    
  
};

RadiationWatch::RadiationWatch(int signPin, int noisePin) : _signPin(signPin), _noisePin(noisePin)
{
  _prevTime = 0;
}

void RadiationWatch::setup()
{
  //PIN setting for Radiation Pulse
  pinMode(_signPin,INPUT);
  digitalWrite(_signPin,HIGH);

  //PIN setting for Noise Pulse
  pinMode(_noisePin,INPUT);
  digitalWrite(_noisePin,HIGH);
  
  //Initialize cpmHistory[]
  for(int i=0; i < kHistoryCount;i++ )
  {
    _cpmHistory[i]=0;
  }
  
  _prevTime = millis();
}

void RadiationWatch::printKey()
{
  //CSV-formatting for serial output (substitute , for _)
  Serial.println("hour[h]_sec[s]_count_cpm_uSv/h_uSv/hError");
}

int RadiationWatch::signPin()
{
  return digitalRead(_signPin);
}

int RadiationWatch::noisePin()
{
  return digitalRead(_noisePin);
}

RadiationWatch radiationWatch(2, 5);



void setup()
{
  //Serial setup
  //9600bps
  Serial.begin(9600);
  
  radiationWatch.setup();

  radiationWatch.printKey();
  
  //Get start time of a loop
  
}

void loop()
{
  // Raw data of Radiation Pulse: Not-detected -> High, Detected -> Low
  int sign = radiationWatch.signPin();

  // Raw data of Noise Pulse: Not-detected -> Low, Detected -> High
  int noise = radiationWatch.noisePin();

  //Radiation Pulse normally keeps low for about 100[usec]
  if(sign==0 && sON==0)
  {//Deactivate Radiation Pulse counting for a while
    sON = 1;
    signCount++;
  }else if(sign==1 && sON==1){
    sON = 0;
  }

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
    currTime = millis();
    
    //No noise detected in 10000 loops
    if(noiseCount == 0)
    {
      //Shift an array for counting log for each 6 sec.
      if( totalSec % 6 == 0 && cpmIndexPrev != totalSec)
      {
        cpmIndexPrev = totalSec;
        cpmIndex++;
        
        if(cpmIndex >= RadiationWatch::kHistoryCount)
        {
          cpmIndex = 0;
        }
        
        if(radiationWatch._cpmHistory[cpmIndex] > 0)
        {
          cpm -= radiationWatch._cpmHistory[cpmIndex];
        }
        radiationWatch._cpmHistory[cpmIndex]=0;
      }
      
      //Store count log
      radiationWatch._cpmHistory[cpmIndex] += signCount;
      //Add number of counts
      cpm += signCount;
      
      //Get ready time for 10000 loops
      cpmTimeMSec += abs(currTime - prevTime);
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
        if(totalSec >= 3600)
        {
          totalSec -= 3600;
          totalHour++;
        }
      }
      
      //Elapsed time of measurement (max=20min.)
      double min = cpmTimeSec / 60.0;
      if(min!=0)
      {
        //Calculate cpm, uSv/h and error of uSv/h
        dtostrf(cpm / min, -1, 3, cpmBuff);
        dtostrf(cpm / min / alpha, -1, 3, uSvBuff);
        dtostrf(sqrt(cpm) / min / alpha, -1, 3, uSvdBuff);
      }else{
        //Devision by zero
        dtostrf(0, -1, 3, cpmBuff);
        dtostrf(0, -1, 3, uSvBuff);
        dtostrf(0, -1, 3, uSvdBuff);
      }
        
      //Create message for serial port
      sprintf(msg, "%d,%d.%03d,%d,%s,%s,%s",
      totalHour,totalSec,
      cpmTimeMSec,
      signCount,
      cpmBuff,
      uSvBuff,
      uSvdBuff
        );
        
      //Send message to serial port
      Serial.println(msg);
      
      index=0;
    }
    
    //Initialization for next 10000 loops
    prevTime = currTime;
    signCount=0;
    noiseCount=0;
  }
  index++;
}


