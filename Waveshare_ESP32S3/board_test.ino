/*
 * Standalone board smoke test, not the complete xDripMon application.
 * Compile this sketch with WaveshareBoard.cpp, WaveshareTime.cpp and
 * WaveshareUi.cpp in the same Arduino sketch folder.
 */
#include "WaveshareBoard.h"
#include "WaveshareTime.h"
#include "WaveshareUi.h"

static WaveshareUi *ui = nullptr;
static uint32_t lastDraw = 0;

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("Waveshare xDripMon hardware-port smoke test");

  if (!board.begin(0)) {
    Serial.println("Board display initialization failed; check the board revision and libraries.");
    while (true) delay(1000);
  }
  waveshareTime.begin();
  ui = new WaveshareUi(board.display());
}

void loop() {
  if (millis() - lastDraw < 1000) return;
  lastDraw = millis();

  struct tm local;
  char clockText[24] = "RTC not set";
  if (waveshareTime.known() && getLocalTime(&local, 10)) {
    snprintf(clockText, sizeof(clockText), "%02d:%02d:%02d",
             local.tm_hour, local.tm_min, local.tm_sec);
  }

  GlucoseView view; // no BLE integration in this board smoke test
  ui->draw(view, "Hardware test", clockText);
  Serial.printf("AXP2101 battery=%d%% charging=%s RTC=%s\n",
                board.batteryPercent(), board.isCharging() ? "yes" : "no",
                waveshareTime.available() ? "ready" : "unavailable");
}
