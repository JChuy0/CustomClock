#include "Clock.h"

RTC_DS1307 rtc;

// TIME DATE VARIABLES
int updateYear = 2025;
int updateMonth = 1;
int updateDay = 1;
int updateClockHour = 0;
int updateClockMinute = 0;
int clockSecond = 0;


void setupRTC() {
  if (!rtc.begin()) {
    Serial.println("Could not find RTC");
    while (1);
  }

  if (!rtc.isrunning()) {
    // Sets time to January 1st, 2025
    rtc.adjust(DateTime(2025, 1, 1, 1, 0, 0));
  }
}

DateTime getCurrentTime() {
  DateTime now = rtc.now(); 
  updateClockHour = now.hour();
  updateClockMinute = now.minute();

  return now;
}

int setClockHour() {
  updateClockHour += encoderValue;
  encoderValue = 0;

  if (updateClockHour < 0) {
    updateClockHour = 23;
  } else if (updateClockHour > 23) {
    updateClockHour = 0;
  }

  return updateClockHour;
}

int setClockMinute() {
  updateClockMinute += encoderValue;
  encoderValue = 0;

  if (updateClockMinute < 0) {
    updateClockMinute = 59;
  } else if (updateClockMinute > 59) {
    updateClockMinute = 0;
  }

  return updateClockMinute;
}

// Updates the time on the clock module
void updateRTC() {
  rtc.adjust(DateTime(updateYear, updateMonth, updateDay, updateClockHour, updateClockMinute, clockSecond));
  // tft.fillScreen(TFT_BLACK);
}
