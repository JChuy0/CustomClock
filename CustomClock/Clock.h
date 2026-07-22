#ifndef CLOCK_H
#define CLOCK_H

#include <RTClib.h>
#include "Data_Types.h"

extern int encoderValue;

void setupRTC();
void getCurrentTime(timeData &clockTime);
void setClockHour(timeData &clockTime);
void setClockMinute(timeData &clockTime);
void updateRTC();

#endif