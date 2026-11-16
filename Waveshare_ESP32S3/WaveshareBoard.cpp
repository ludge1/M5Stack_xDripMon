#include "WaveshareBoard.h"

namespace {
constexpr int PIN_LCD_BL = 6;
constexpr int PIN_SPI_MISO = 2;
constexpr int PIN_SPI_MOSI = 1;
constexpr int PIN_SPI_SCLK = 5;
constexpr int PIN_LCD_DC = 3;
constexpr int PIN_LCD_CS = -1;
constexpr int PIN_LCD_RST = -1;
constexpr int PIN_I2C_SDA = 8;
constexpr int PIN_I2C_SCL = 7;
constexpr int LCD_WIDTH = 320;
constexpr int LCD_HEIGHT = 480;
constexpr uint8_t LCD_RESET_EXPANDER_PIN = 1;
}

WaveshareBoard board;

bool WaveshareBoard::begin(uint8_t rotation) {
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);

  // TCA9554 output 1 controls the LCD reset line on Waveshare's reference
  // design. Keep this reset sequence aligned with their Arduino GFX example.
  if (!expander.begin()) {
    Serial.println("[board] TCA9554 init failed");
    return false;
  }
  expander.pinMode1(LCD_RESET_EXPANDER_PIN, OUTPUT);
  expander.write1(LCD_RESET_EXPANDER_PIN, 1);
  delay(10);
  expander.write1(LCD_RESET_EXPANDER_PIN, 0);
  delay(10);
  expander.write1(LCD_RESET_EXPANDER_PIN, 1);
  delay(200);

  bus = new Arduino_ESP32SPI(PIN_LCD_DC, PIN_LCD_CS,
                             PIN_SPI_SCLK, PIN_SPI_MOSI, PIN_SPI_MISO);
  gfx = new Arduino_ST7796(bus, PIN_LCD_RST, rotation, true,
                           LCD_WIDTH, LCD_HEIGHT);
  if (!gfx->begin()) {
    Serial.println("[board] ST7796 init failed");
    return false;
  }

  pinMode(PIN_LCD_BL, OUTPUT);
  digitalWrite(PIN_LCD_BL, HIGH);
  gfx->fillScreen(BLACK);

  // Waveshare's board examples share this I2C bus for the PMIC and RTC.
  pmuReady = pmu.begin(Wire, AXP2101_SLAVE_ADDRESS,
                       PIN_I2C_SDA, PIN_I2C_SCL);
  Serial.printf("[board] AXP2101: %s\n", pmuReady ? "ready" : "not found");
  return true;
}

int WaveshareBoard::batteryPercent() const {
  if (!pmuReady || !pmu.isBatteryConnect()) return -1;
  return pmu.getBatteryPercent();
}

bool WaveshareBoard::isCharging() const {
  return pmuReady && pmu.isCharging();
}
