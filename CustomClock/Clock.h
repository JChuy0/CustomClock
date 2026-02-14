#ifndef CLOCK_H
#define CLOCK_H

#include "RTClib.h"
#include "TFT_eSPI.h"

extern TFT_eSPI tft;

// extern bool editModeEnabled;
extern int encoderValue;

void setupRTC();

DateTime getCurrentTime();
int setClockHour();
int setClockMinute();
void updateRTC();


#endif