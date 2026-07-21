#ifndef CLOCK_H
#define CLOCK_H

#include <RTClib.h>
#include "Data_Types.h"

extern int encoderValue;
extern timeData clockTime;

void setupRTC();
void getCurrentTime();
int setClockHour();
int setClockMinute();
void updateRTC();

#endif