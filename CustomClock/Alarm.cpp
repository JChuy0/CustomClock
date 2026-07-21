#include "Alarm.h"

int updateAlarmHour = 0;
int updateAlarmMinute = 0;
bool isAlarmOn = false;

AudioGeneratorMP3 *mp3;
AudioFileSourceLittleFS *file;
AudioOutputI2S *out;


void testAlarm() {
  // year month day hour minute second
  rtc.adjust(DateTime(2025, 1, 1, 18, 44, 50));

  updateAlarmHour = 18;
  updateAlarmMinute = 45;

  isAlarmOn = true;
}


void alarmSetup() {
  file = new AudioFileSourceLittleFS("/alarm.mp3");
  out = new AudioOutputI2S();
  out->SetPinout(33, 25, 32);  // BCLK, LRC, DOUT
  out->SetGain(0.5);  // changes volume
  mp3 = new AudioGeneratorMP3();
  mp3->begin(file, out);
}


int setAlarmHour() {
  updateAlarmHour += encoderValue;
  encoderValue = 0;

  if (updateAlarmHour < 0) {
    updateAlarmHour = 23;
  } else if (updateAlarmHour > 23) {
    updateAlarmHour = 0;
  }

  return updateAlarmHour;
}

int setAlarmMinute() {
  updateAlarmMinute += encoderValue;
  encoderValue = 0;

  if (updateAlarmMinute < 0) {
    updateAlarmMinute = 59;
  } else if (updateAlarmMinute > 59) {
    updateAlarmMinute = 0;
  }

  return updateAlarmMinute;
}

// Checks if it's time for the alarm to go off
void checkAlarm() {
  DateTime now = rtc.now();

  // The alarm will continue to ring until it is turned off
  if (isAlarmOn && (now.hour() == updateAlarmHour) && (now.minute() == updateAlarmMinute) && (now.second() == 0) ) {
    isAlarmRinging = true;
  }

  if (isAlarmRinging) {
    alarmBuzzer();
  } else {
    mp3->stop();
  }
}

void alarmBuzzer() {
  if (mp3->isRunning()) {
    if (!mp3->loop()) {
      mp3->stop();
    }
  } else {
    delay(2000);

    file = new AudioFileSourceLittleFS("/alarm.mp3");
    mp3->begin(file, out);
  }
}

bool turnAlarmOnOff() {
  if (encoderValue != 0) {
    isAlarmOn = !isAlarmOn;
    encoderValue = 0;
  }

  return isAlarmOn;
}

