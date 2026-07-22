#ifndef ALARM_H
#define ALARM_H

#include <SPI.h>
#include <RTClib.h>
#include <AudioFileSourceLittleFS.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>
#include "Data_Types.h"


extern RTC_DS1307 rtc;

extern bool isAlarmRinging;
extern int encoderValue;

void alarmSetup();
void setAlarmHour(timeData &alarmTime);
void setAlarmMinute(timeData &alarmTime);

void checkAlarm();
void alarmBuzzer();
bool turnAlarmOnOff();

void testAlarm();

#endif