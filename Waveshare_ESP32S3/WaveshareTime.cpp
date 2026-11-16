#include "WaveshareTime.h"
#include <Wire.h>
#include <sys/time.h>
#include "SensorPCF85063.hpp"

namespace {
SensorPCF85063 rtc;
}

WaveshareTime waveshareTime;

bool WaveshareTime::begin() {
  // Waveshare board I2C pins: SDA=8, SCL=7. The board layer may have already
  // called Wire.begin(); repeating it with the same pins is intentional here
  // so this service can also be tested independently.
  Wire.begin(8, 7);
  rtcReady = rtc.begin(Wire);
  if (!rtcReady) {
    Serial.println("[time] PCF85063 not detected");
    return false;
  }

  RTC_DateTime dt = rtc.getDateTime();
  if (dt.getYear() < 2024 || dt.getYear() > 2099) {
    Serial.println("[time] RTC has no plausible date yet");
    return false;
  }

  struct tm t = {};
  t.tm_year = dt.getYear() - 1900;
  t.tm_mon = dt.getMonth() - 1;
  t.tm_mday = dt.getDay();
  t.tm_hour = dt.getHour();
  t.tm_min = dt.getMinute();
  t.tm_sec = dt.getSecond();
  time_t utc = timegm(&t);
  struct timeval tv = { .tv_sec = utc, .tv_usec = 0 };
  settimeofday(&tv, nullptr);
  clockKnown = true;
  Serial.println("[time] system clock restored from PCF85063 (UTC)");
  return true;
}

bool WaveshareTime::setUtc(time_t utc) {
  if (utc < 1704067200 || utc > 4102444799LL) return false;
  struct timeval tv = { .tv_sec = utc, .tv_usec = 0 };
  settimeofday(&tv, nullptr);
  clockKnown = true;
  return syncRtcFromSystem();
}

bool WaveshareTime::syncRtcFromSystem() {
  if (!rtcReady) return false;
  time_t now = time(nullptr);
  if (now < 1704067200 || now > 4102444799LL) return false;
  struct tm t;
  gmtime_r(&now, &t);
  rtc.setDateTime(t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                  t.tm_hour, t.tm_min, t.tm_sec);
  clockKnown = true;
  return true;
}
