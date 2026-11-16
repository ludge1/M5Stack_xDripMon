#include "WaveshareUi.h"

namespace {
constexpr uint16_t C_BG = 0x0000;
constexpr uint16_t C_WHITE = 0xFFFF;
constexpr uint16_t C_GREY = 0x8410;
constexpr uint16_t C_CYAN = 0x07FF;
constexpr uint16_t C_GREEN = 0x07E0;
constexpr uint16_t C_YELLOW = 0xFFE0;
constexpr uint16_t C_RED = 0xF800;
constexpr uint16_t C_BLUE = 0x33DF;

uint16_t glucoseColor(const GlucoseView &v) {
  if (v.alarm) return C_RED;
  if (!v.hasReading) return C_GREY;
  return C_GREEN;
}
}

void WaveshareUi::draw(const GlucoseView &v, const char *source,
                       const char *clockText) {
  gfx.fillScreen(C_BG);
  gfx.setTextWrap(false);

  // Header
  gfx.setTextSize(2);
  gfx.setTextColor(C_WHITE, C_BG);
  gfx.setCursor(12, 12);
  gfx.print(clockText ? clockText : "--:--");
  gfx.setCursor(12, 42);
  gfx.setTextColor(C_CYAN, C_BG);
  gfx.print(source ? source : "xDrip monitor");

  gfx.setTextSize(2);
  gfx.setCursor(12, 88);
  gfx.setTextColor(v.authenticated ? C_BLUE : (v.connected ? C_YELLOW : C_RED), C_BG);
  gfx.print(v.authenticated ? "BLE ready" : (v.connected ? "BLE pairing" : "BLE waiting"));

  // Main value: use the built-in font for broad Arduino_GFX compatibility.
  gfx.setTextColor(glucoseColor(v), C_BG);
  gfx.setTextSize(6);
  gfx.setCursor(12, 150);
  if (v.hasReading) {
    if (v.unitsMgdl) gfx.printf("%u", (unsigned)lroundf(v.value));
    else gfx.printf("%.1f", v.value);
  } else {
    gfx.print("---");
  }

  gfx.setTextSize(2);
  gfx.setCursor(14, 220);
  gfx.setTextColor(C_WHITE, C_BG);
  gfx.print(v.unitsMgdl ? "mg/dL" : "mmol/L");
  gfx.setCursor(150, 220);
  gfx.setTextColor(C_GREY, C_BG);
  if (v.deltaValid) gfx.printf("Delta %d", v.delta);
  else gfx.print("Delta --");

  gfx.drawFastHLine(12, 268, 296, C_GREY);
  gfx.setCursor(14, 286);
  gfx.setTextColor(C_WHITE, C_BG);
  if (v.minutesAgo >= 0) gfx.printf("Reading age: %d min", v.minutesAgo);
  else gfx.print("Waiting for first reading");

  gfx.setCursor(14, 330);
  gfx.setTextColor(C_WHITE, C_BG);
  gfx.print("Trend:");
  if (v.trendAngle == 180) {
    gfx.print(" --");
  } else {
    gfx.printf(" %d deg", v.trendAngle);
  }

  gfx.drawFastHLine(12, 390, 296, C_GREY);
  gfx.setTextSize(1);
  gfx.setTextColor(C_GREY, C_BG);
  gfx.setCursor(12, 408);
  gfx.print("Waveshare ESP32-S3 / ST7796");
  gfx.setCursor(12, 426);
  gfx.print("For display/testing only; not for treatment decisions.");
}
