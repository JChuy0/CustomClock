#ifndef ALARM_H
#define ALARM_H

#include "RTClib.h"
extern RTC_DS1307 rtc;

// extern bool isActive;
extern int encoderValue;

// extern int alarmHour;
// extern int alarmMinute;

// void setupRTC();

int setAlarmHour();
int setAlarmMinute();

void checkAlarm();
void alarmBuzzer();
void turnAlarmOnOff();

#endif