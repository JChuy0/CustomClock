#ifndef CLOCK_AND_ALARM_H
#define CLOCK_AND_ALARM_H

#include "TFT_eSPI.h"
extern TFT_eSPI tft;

extern bool isActive;
extern int encoderValue;

extern int updateClockHour;
extern int updateClockMinute;
extern int clockSecond;
extern int alarmHour;
extern int alarmMinute;

void setupRTC();

void getCurrentTime();
void setTime(int &editingValue, int maxValue, const char* label, int displayValue);
void updateRTC();
void displayClock();

void checkAlarm();
void alarmBuzzer();
void turnAlarmOnOff();

#endif