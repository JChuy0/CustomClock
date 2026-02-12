#include "Clock.h"
#include "RTClib.h"
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

  // isAlarmOn = true;
}

void getCurrentTime() {
  DateTime now = rtc.now(); 
  updateClockHour = now.hour();
  updateClockMinute = now.minute();
  tft.fillScreen(TFT_BLACK);
}


/*
set time (string "hour/minute")

run getCurrentTime();
  this puts the current time into hour/minute variables

  if (string == hour) {
    updateClockHour += encoderValue;
    maxValue = 23;
  } else if (string == minute) {
    updateClockMinute += encoderValue;
    maxValue = 59;
  }

  encoderValue = 0;

  if (value < 0) {
    value = maxValue;
  } else if (value > maxValue) {
    value = 0;
  }


*/


int setClockHour() {
  if(!editModeInitialized) {
    getCurrentTime();
    editModeInitialized = true;
  }

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


// A generic function to set any time value (hours or minutes)
// value: The value being adjusted (hour or minute).
// maxValue: Upper limit. 23 for hour, 59 for minute.
// label: Screen label (e.g. "Set alarm hour").
// otherValue: The paired time value shown for context .
// void setTime(int &value, int maxValue, const char* label, int otherValue) {
//   value += encoderValue;
//   encoderValue = 0;

//   if (value < 0) {
//     value = maxValue;
//   } else if (value > maxValue) {
//     value = 0;
//   }

//   tft.setTextSize(2);
//   char timeStr[9];

//   if (maxValue == 23) {
//     sprintf(timeStr, "%02d:%02d", value, otherValue);
//   } else if (maxValue == 59) {
//     sprintf(timeStr, "%02d:%02d", otherValue, value);
//   }

//   tft.setTextColor(TFT_WHITE, TFT_BLACK);
//   tft.drawString(label, 0, 200);
//   tft.drawString(timeStr, 0, 250, 4);
// }


// Updates the time on the clock module
void updateRTC() {
  rtc.adjust(DateTime(updateYear, updateMonth, updateDay, updateClockHour, updateClockMinute, clockSecond));
  tft.fillScreen(TFT_BLACK);
}


// void displayClock(){
//   DateTime now = rtc.now();

//   tft.setTextSize(2);

//   char timeStr[9];
//   sprintf(timeStr, "%2d:%02d:%02d", now.hour(), now.minute(), now.second());

//   tft.setTextColor(TFT_WHITE, TFT_BLACK);
//   tft.drawString(timeStr, 0, 200, 4);

//   // Display if alarm is on or off
//   if (isAlarmOn) {
//     tft.drawString("On", 300, 200, 4);
//   } else {
//     tft.drawString("Off", 300, 200, 4);
//   }
// }

