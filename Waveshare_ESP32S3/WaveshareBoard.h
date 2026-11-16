#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <TCA9554.h>
#include <Wire.h>

// Optional AXP2101 telemetry. This layer only reads battery and charge state;
// it intentionally does not alter PMIC regulator or charger settings.
#define XPOWERS_CHIP_AXP2101
#include <XPowersLib.h>

class WaveshareBoard {
public:
  bool begin(uint8_t rotation = 0);
  Arduino_GFX &display() { return *gfx; }
  bool pmuAvailable() const { return pmuReady; }
  int batteryPercent() const;
  bool isCharging() const;

private:
  TCA9554 expander{0x20};
  XPowersPMU pmu;
  Arduino_DataBus *bus = nullptr;
  Arduino_GFX *gfx = nullptr;
  bool pmuReady = false;
};

extern WaveshareBoard board;
