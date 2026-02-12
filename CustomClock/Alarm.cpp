#include "Alarm.h"
#include "RTClib.h"

int updateAlarmHour = 0;
int updateAlarmMinute = 0;

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


// void testAlarm() {
//   // year month day hour minute second
//   rtc.adjust(DateTime(2025, 1, 1, 18, 44, 40));

//   alarmHour = 17;
//   alarmMinute = 45;
// }

int SetAlarmHour() {
  updateAlarmHour += encoderValue;
  encoderValue = 0;

  if (updateAlarmHour < 0) {
    updateAlarmHour = 23;
  } else if (updateAlarmHour > 23) {
    updateAlarmHour = 0;
  }

  return updateAlarmHour;
}

int SetAlarmMinute() {
  updateAlarmMinute += encoderValue;
  encoderValue = 0;

  if (updateAlarmMinute < 0) {
    updateAlarmMinute = 59;
  } else if (updateAlarmMinute > 59) {
    updateAlarmMinute = 0;
  }

  return updateAlarmMinute;
}


void checkAlarm() {
  DateTime now = rtc.now();

  // I coded things this way so the alarm will continue to ring until it is turned off.
  if (isAlarmOn && (now.hour() == updateAlarmHour) && (now.minute() == updateAlarmMinute) && (now.second() == 0) ) {
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

  // tft.setTextColor(TFT_WHITE, TFT_BLACK);
  // tft.drawString("Alarm is:", 0, 0, 2);

  // if (isAlarmOn) {
  //   tft.drawString("On", 0, 100, 2);
  // } else {
  //   tft.drawString("Off", 0, 100, 2);
  // }
}