#ifndef CLOCK_H
#define CLOCK_H

#include "RTClib.h"

extern int encoderValue;

void setupRTC();
DateTime getCurrentTime();
int setClockHour();
int setClockMinute();
void updateRTC();

#endif