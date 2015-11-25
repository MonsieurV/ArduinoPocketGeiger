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
#define HISTORY_LENGTH 200
#define PROCESS_PERIOD 160
#include "Arduino.h"

class RadiationWatch
{
  public:
    RadiationWatch(byte signPin, byte noisePin, byte signIrq, byte noiseIrq);

    void setup();
    void loop();

    // Return the duration of the measurement (ms).
    unsigned long duration();
    // Return the number of radiation count by minute.
    float cpm();
    // Return the radiation dose, exprimed in Sievert (uSv/h).
    float uSvh();
    /* Return the error of the measurement (uSv/h).
     * The range of precision of the measurement is:
     * [ uSvh-uSvhError, uSvh+uSvhError ]. */
    float uSvhError();

    /* Register a function that will be called when a radiation pulse
     * is detected. */
    void registerRadiationCallback(void (*callback)(void));
    /* Register a function that will be called when a noise pulse
     * is detected. */
    void registerNoiseCallback(void (*callback)(void));

    // Get CSV-formatted keys for the status values.
    char* csvKeys();
    // Get CSV-formatted current values.
    char* csvStatus();

  protected:
    // Process the max CPM time from the HISTORY_LENGTH:
    // HISTORY_LENGTH * 6 / 60 * 60 * 1000 (currently 20 minutes)
    static const unsigned long maxCpmTime = HISTORY_LENGTH * 6 * 20 * 1000L;
    // History of count rates.
    float _cpmHistory[HISTORY_LENGTH];
    unsigned long previousTime;
    // Count rate [cpm] of current.
    float _cpm;
    // Position of current count rate on cpmHistory[].
    byte cpmIndex;
    // Flag to prevent duplicative counting.
    byte cpmIndexPrev;
    // Elapsed time of measurement (milliseconds).
    unsigned long totalTime;
    // Elapsed time of measurement used for CPM calculation (in minutes).
    inline float cpmTime()
    {
      return ((long) min(totalTime, maxCpmTime) / 1000) / 60.0;
    }
    // Pin settings.
    byte _signPin;
    byte _noisePin;
    byte _signIrq;
    byte _noiseIrq;
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
