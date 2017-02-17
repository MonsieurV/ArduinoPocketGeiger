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
#ifndef RadiationWatch_h
#define RadiationWatch_h
/* Number of cells of the history array. If you do not have
 * memory issues, it is adviced to keep the default value (200), for an
 * history duration of 20 minutes (200 * 6 seconds). */
#ifndef HISTORY_LENGTH
#define HISTORY_LENGTH 200
#endif
// Duration of each history array cell (seconds).
#define HISTORY_UNIT 6
#define PROCESS_PERIOD 160
#include "Arduino.h"

class RadiationWatch
{
  public:
    /* signPin: Number of the pin to which is connected the signal wire.
     * noisePin: Number of the pin ot which is connected the noise wire. */
    RadiationWatch(byte signPin = 2, byte noisePin = 3);

    void setup();
    void loop();

    // Return the duration of the measurement (ms).
    unsigned long duration();
    /* Return the current radiation count, that is the number of Gamma ray
     * since the last call to loop(), which reset the current count to 0. */
    int currentRadiationCount();
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
    // History of count rates.
    unsigned int _cpmHistory[HISTORY_LENGTH];
    unsigned long previousTime;
    unsigned long previousHistoryTime;
    // Current count per minute (CPM).
    unsigned long _cpm;
    // Position of current count rate on cpmHistory[].
    byte cpmIndex;
    // Current length of count history
    byte historyLength;
    // Elapsed time of measurement (milliseconds).
    // Will overflow after days 49 of measurement.
    unsigned long totalTime;
    // Elapsed time of measurement used for CPM calculation (in minutes).
    inline float cpmTime()
    {
      return (historyLength * HISTORY_UNIT
              + (previousTime - previousHistoryTime) / 1000.0) / 60.0;
    }
    // Pin settings.
    byte _signPin;
    byte _noisePin;
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
