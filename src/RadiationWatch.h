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

    /* Integration time of traced radiation count (in milliseconds),
     * grows gradually to HISTORY_LENGTH * HISTORY_UNIT * 1000. */
    unsigned long integrationTime();
    /* Return the current radiation count, that is the number of Gamma ray
     * since the last call to loop(), which reset the current count to 0. */
    int currentRadiationCount();
    /* Return the radiation count, that is the number of Gamma ray occured
     * during the integration time. */
    unsigned long radiationCount();
    // Return the number of radiation count by minute.
    double cpm();
    // Return the radiation dose, exprimed in Sievert (uSv/h).
    double uSvh();
    /* Return the error of the measurement (uSv/h).
     * The range of precision of the measurement is:
     * [ uSvh-uSvhError, uSvh+uSvhError ]. */
    double uSvhError();
    // Dose coefficient (cpm = uSv x alpha)
#if __cplusplus >= 200704 // constexpr is requiered on C++11
    static constexpr double kAlpha = 53.032;
#else
    static const double kAlpha = 53.032;
#endif

    /* Register a function that will be called when a radiation pulse
     * is detected. */
    void registerRadiationCallback(void (*callback)(void));
    /* Register a function that will be called when a noise pulse
     * is detected. */
    void registerNoiseCallback(void (*callback)(void));

  protected:
    // History of count rates.
    unsigned int _countHistory[HISTORY_LENGTH];
    unsigned long previousTime;
    unsigned long previousHistoryTime;
    // Current count (sum of count in _countHistory).
    unsigned long _count;
    // Position of current count rate on _countHistory[].
    byte historyIndex;
    // Current length of count history
    byte historyLength;
    // Pin settings.
    byte _signPin;
    byte _noisePin;
    // User callbacks.
    void (*_radiationCallback)(void);
    void (*_noiseCallback)(void);
    // radiation count used in interrupt routine
    static int volatile _radiationCount;
    // noise count used in interrupt routine
    static int volatile _noiseCount;
    // interrupt handler
    static void _onRadiationHandler();
    static void _onNoiseHandler();
};
#endif
