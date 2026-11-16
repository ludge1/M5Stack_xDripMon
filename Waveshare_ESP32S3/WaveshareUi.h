#pragma once
#include <Arduino.h>
#include <Arduino_GFX_Library.h>

struct GlucoseView {
  bool hasReading = false;
  float value = 0.0f;
  bool unitsMgdl = true;
  int delta = 0;
  bool deltaValid = false;
  int minutesAgo = -1;
  int trendAngle = 180; // 180 hides trend arrow
  bool connected = false;
  bool authenticated = false;
  bool alarm = false;
};

class WaveshareUi {
public:
  explicit WaveshareUi(Arduino_GFX &display) : gfx(display) {}
  void draw(const GlucoseView &view, const char *source, const char *clockText);

private:
  Arduino_GFX &gfx;
};
