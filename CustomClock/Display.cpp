#include "Display.h"

TFT_eSPI tft = TFT_eSPI();

void displayTimeEditor(timeData &time, const char* label = "");
void displayAlarmOnOff(bool alarmOnOff, const char* label = "");


void setup_TFT_Screen() {
  tft.init();

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setRotation(3);
}

void resetScreen() {
  tft.fillScreen(TFT_BLACK);
}


void displayClock(DateTime currTime, BME680Data airData, bool alarmOnOffValue) {
  Serial.println("Entered displayClock.");

  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // Air Sensor Data
  char temperature[20];
  char humidity[20];

  sprintf(temperature, "%.1f C", airData.temperature);
  sprintf(humidity, "%.1f %", airData.humidity);

  // Temperature
  tft.drawString("Temperature:", 0, 0);
  tft.drawString(temperature, 25, 20, 2);

  // Humidity
  tft.drawString("Humidity:", 180, 0);
  tft.drawString(humidity, 195, 20, 2);

  // Clock
  char timeStr[10];
  sprintf(timeStr, "%02d:%02d:%02d", currTime.hour(), currTime.minute(), currTime.second());

  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(timeStr, 0, 250, 4);

  // Alarm
  displayAlarmOnOff(alarmOnOffValue);
}

void displayTimeEditor(timeData &time, const char* label) {

  // Time
  char timeStr[9];
  sprintf(timeStr, "%02d:%02d", time.hour, time.minute);

  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(label, 0, 200);
  tft.drawString(timeStr, 0, 250, 4);
}

void displayAlarmOnOff(bool alarmOnOff, const char* label) {
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(label, 250, 200, 2);

  if (alarmOnOff) {
    tft.drawString("On  ", 250, 250, 2);
  } else {
    tft.drawString("Off", 250, 250, 2);
  }
}
