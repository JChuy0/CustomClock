

#include "ClockAndAlarm.h"
#include "RTClib.h"
RTC_DS1307 rtc;

// TIME DATE VARIABLES
int updateYear = 2025;
int updateMonth = 1;
int updateDay = 1;
// int updateClockHour = 0;
// int updateClockMinute = 0;
// int clockSecond = 0;
// int alarmHour = 0;
// int alarmMinute = 0;

bool isAlarmOn = false;
bool isAlarmRinging = false;


// Set up the notes and melody
#define NOTE_C4  262
#define NOTE_G3  196

int noteDurations[] = {
  4, 8, 4, 8, 4, 8, 4, 8
};

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_C4, NOTE_G3, NOTE_C4, NOTE_G3, NOTE_C4, NOTE_G3
};


void setupRTC() {
  if (!rtc.begin()) {
    Serial.println("Could not find RTC");
    while (1);
  }

  if (!rtc.isrunning()) {
    // Sets time to January 1st, 2025
    rtc.adjust(DateTime(2025, 1, 1, 1, 0, 0));
  }

  isAlarmOn = true;
}

// void getCurrentTime() {
//   if (isActive) {
//     DateTime now = rtc.now(); 
//     updateClockHour = now.hour();
//     updateClockMinute = now.minute();
//     isActive = false;
//     tft.fillScreen(TFT_BLACK);
//   }
// }

// A generic function to set any time value (hours or minutes)
// editingValue: The value being adjusted (hour or minute).
// maxValue: Upper limit. 23 for hour, 59 for minute.
// label: Screen label (e.g. "Set alarm hour").
// displayValue: The paired time value shown for context .
void setTime(int &value, int maxValue, const char* label, int otherValue) {
  value += encoderValue;
  encoderValue = 0;

  if (value < 0) {
    value = maxValue;
  } else if (value > maxValue) {
    value = 0;
  }

  tft.setTextSize(2);
  char timeStr[9];

  if (maxValue == 23) {
    sprintf(timeStr, "%02d:%02d", value, otherValue);
  } else if (maxValue == 59) {
    sprintf(timeStr, "%02d:%02d", otherValue, value);
  }

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(label, 0, 200);
  tft.drawString(timeStr, 0, 250, 4);
}


// Updates the time on the clock module
void updateRTC() {
  rtc.adjust(DateTime(updateYear, updateMonth, updateDay, updateClockHour, updateClockMinute, clockSecond));
  tft.fillScreen(TFT_BLACK);
}


void displayClock(){
  DateTime now = rtc.now();

  tft.setTextSize(2);

  char timeStr[9];
  sprintf(timeStr, "%2d:%02d:%02d", now.hour(), now.minute(), now.second());

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(timeStr, 0, 200, 4);

  // Display if alarm is on or off
  if (isAlarmOn) {
    tft.drawString("On", 300, 200, 4);
  } else {
    tft.drawString("Off", 300, 200, 4);
  }
}



void testAlarm() {
  // year month day hour minute second
  rtc.adjust(DateTime(2025, 1, 1, 18, 44, 40));

  alarmHour = 17;
  alarmMinute = 45;
}



void checkAlarm() {
  DateTime now = rtc.now();

  // I coded things this way so the alarm will continue to ring until it is turned off.
  if (isAlarmOn && (now.hour() == alarmHour) && (now.minute() == alarmMinute) && (now.second() == 0) ) {
    isAlarmRinging = true;
  }

  if (isAlarmRinging) {
    alarmBuzzer();
  }
}

void alarmBuzzer() {
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];

    // pin number, sound to play, duration
    tone(33, melody[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(33);
  }
}


void turnAlarmOnOff() {
  if (encoderValue != 0) {
    isAlarmOn = !isAlarmOn;
    encoderValue = 0;
  }

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Alarm is:", 0, 0, 2);

  if (isAlarmOn) {
    tft.drawString("On", 0, 100, 2);
  } else {
    tft.drawString("Off", 0, 100, 2);
  }
}




