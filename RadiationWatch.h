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
#ifndef RadiationWatch_h
#define RadiationWatch_h

/*
TODO:
- Code a clearer time management
  (separate time as hours and seconds is great for priting,
  but not handy as a programmable interface)
- Allows for a periodic status printer callback?
  (note: this can be done easily outside the lib, so why bother? Why adding clubersome?)
- On the loop(), update the stats from the time ellasped, not a number of loops which
  can sensibly varies by the work done outside. Maybe do not do a busy loop?
  (again, the caller can easily turn the busy loop to a time-triggered loop if
  we rely on the time ellasped between each call)
- Code the examples: SerialCsvLogger, SdCardLogger, HttpJsonLogger
- A a LICENSE file to the code
  (like https://github.com/MonsieurV/ArduinoTimeInterruptHell/blob/master/LICENSE)
  Maybe ask thomasaw and RadiationWatch? Or refactor the code enough.
*/

class RadiationWatch
{
  public:
    RadiationWatch(int signPin, int noisePin, int signIrq, int noiseIrq);

    void setup();
    void loop();

    // Return the duration of the measurement (ms).
    unsigned long duration();
    // Return the number of radiation count by minute.
    double cpm();
    // Return the radiation dose, exprimed in Sievert (uSv/h).
    double uSvh();
    /* Return the error of the measurement (uSv/h).
     * The range of precision of the measurement is:
     * [ uSvh-uSvhError, uSvh+uSvhError ]. */
    double uSvhError();

    /* Register a function that will be called when a radiation pulse
     * is detected. */
    void registerRadiationCallback(void (*callback)(void));
    /* Register a function that will be called when a noise pulse
     * is detected. */
    void registerNoiseCallback(void (*callback)(void));

    char* csvKeys();
    char* csvStatus();

  protected:
    double cpmTime();
    static const unsigned int kHistoryCount = 200;
    // History of count rates.
    double _cpmHistory[kHistoryCount];
    int _prevTime;
    // Number of loops.
    int index;
    // Count rate [cpm] of current.
    double _cpm;
    // Position of current count rate on cpmHistory[].
    int cpmIndex;
    // Flag to prevent duplicative counting.
    int cpmIndexPrev;
    // Elapsed time of measurement (milliseconds).
    unsigned long _duration;
    // Elapsed time of measurement (seconds).
    int totalSec;
    // Elapsed time of measurement (hours).
    int totalHour;
    // Time settings for CPM calcuaration.
    int cpmTimeMSec;
    int cpmTimeSec;
    // Pin settings.
    int _signPin;
    int _noisePin;
    int _signIrq;
    int _noiseIrq;
    // User callbacks.
    void (*_radiationCallback)(void);
    void (*_noiseCallback)(void);

  public:
    /* Functions called by hardware external interrupts.
     * We put them here, as public members, in order to call them through
     * the Interrupt Service Routines (ISR), which can not be class members.
     * <!> Do NOT call them or the results will be fasified. <!>
     */
    void _onRadiation();
    void _onNoise();
};
#endif
