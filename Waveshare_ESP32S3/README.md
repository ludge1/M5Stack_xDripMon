# Waveshare ESP32-S3-Touch-LCD-3.5 board port

This folder adds a board-specific hardware layer for the Waveshare ESP32-S3-Touch-LCD-3.5 / 3.5-C (320×480, ST7796, AXP2101, PCF85063, TCA9554). FT6336 is deliberately unused.

## Status and scope

This is the hardware-port foundation and a standalone board smoke test, **not yet a complete build of the xDripMon application**. The parent sketch and application modules still call M5Unified for rendering, buttons, sound, power and RTC. Those modules need to be migrated to this layer before the full BLE monitor can be flashed. The BLE protocols themselves do not depend on touch and can be retained.

Files:

- `WaveshareBoard.h/.cpp` — ST7796 setup/reset, backlight, and read-only AXP2101 battery/charging queries.
- `WaveshareTime.h/.cpp` — PCF85063-to-system-clock sync and UTC writeback.
- `WaveshareUi.h/.cpp` — small Arduino_GFX dashboard renderer in portrait orientation; no touch dependency.
- `board_test.ino` — standalone hardware test that initializes LCD/RTC/PMIC and refreshes the screen. Keep it in its own Arduino sketch folder when compiling.

## Verified board wiring

Pin and initialization values follow Waveshare's official `08_gfx_helloworld` and `05_pcf85063_time` Arduino examples:

| Function | Pin / address |
| --- | --- |
| SPI SCLK / MOSI / MISO | GPIO 5 / 1 / 2 |
| LCD DC / CS / RST | GPIO 3 / not connected / not connected |
| LCD backlight | GPIO 6, active high |
| LCD reset | TCA9554 output 1, address 0x20 |
| I2C SDA / SCL | GPIO 8 / 7 |
| Resolution | 320×480 |
| Panel | ST7796 |
| PMIC | AXP2101 |
| RTC | PCF85063 |

## Arduino setup

Use Waveshare's current Arduino setup for this product: **ESP32S3 Dev Module**, 16 MB flash, OPI PSRAM, QIO flash mode, and the 16 MB partition option described in their repository. Install the Arduino libraries used by Waveshare examples: **GFX Library for Arduino**, **TCA9554**, **XPowersLib**, and **SensorLib** (for `SensorPCF85063.hpp`). The original monitor additionally needs **NimBLE-Arduino 2.x** and **Adafruit NeoPixel**.

Compile `board_test.ino` as a sketch on its own first. It starts the I2C bus, releases the LCD reset through the expander, initializes the display and attempts to sync the system clock from the RTC. PMIC setup is read-only: it does not change regulator voltages or charger configuration.

## Integrating the existing monitor

1. Replace `M5Unified.h` and `M5.begin()` in the main sketch with `WaveshareBoard::begin()`; do not initialize FT6336.
2. Replace `M5.Lcd` drawing calls in `Ui.cpp` and `Menu.cpp` with the `Arduino_GFX` API / `WaveshareUi`. Re-layout for 320×480. M5 `Free_Fonts.h` and `setFreeFont()` are not part of this display backend.
3. Replace M5 A/B/C button control with the chosen non-touch control. This port does not assume physical buttons; serial/control integration is still needed for page changes, menus, source selection and snooze.
4. Replace `M5.Rtc` in `TimeService.cpp` with `WaveshareTime`; the PCF85063 RTC keeps UTC across reset/power loss.
5. Replace `M5.Power` battery and charging calls with `WaveshareBoard::batteryPercent()` and `isCharging()`. `setBrightness()` must be adapted to the GPIO 6 backlight (simple on/off unless a PWM-capable backlight circuit is confirmed).
6. Replace M5 speaker calls in `Alarms.cpp`. The board's audio codec is not configured by this initial port; do not assume `M5.Speaker` will work.
7. Remove M5 board detection and Fire LED-bar handling from `LedStrip.cpp`. Configure external NeoPixel pin/count only after selecting a GPIO that is not used by the board.
8. Build and verify the board smoke test, then integrate and test BLE, RTC sync, alarms, display rotation, and input separately. This project is a monitor, not a medical device; do not use it to make treatment decisions.

The Waveshare source reference is <https://github.com/waveshareteam/ESP32-S3-Touch-LCD-3.5>.
