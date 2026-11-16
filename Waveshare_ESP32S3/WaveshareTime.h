#pragma once
#include <Arduino.h>
#include <time.h>

class WaveshareTime {
public:
  bool begin();                    // initialize PCF85063 and restore system UTC
  bool available() const { return rtcReady; }
  bool known() const { return clockKnown; }
  bool setUtc(time_t utc);         // set system clock and PCF85063 in UTC
  bool syncRtcFromSystem();        // write current system UTC to RTC

private:
  bool rtcReady = false;
  bool clockKnown = false;
};

extern WaveshareTime waveshareTime;
