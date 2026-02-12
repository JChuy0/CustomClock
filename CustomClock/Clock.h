#ifndef CLOCK_H
#define CLOCK_H

#include "TFT_eSPI.h"
extern TFT_eSPI tft;

extern bool editModeInitialized;
extern int encoderValue;

extern int updateClockHour;
extern int updateClockMinute;
extern int clockSecond;

void setupRTC();

// void getCurrentTime();
int setClockHour();
int setClockMinute();

void setTime(int &editingValue, int maxValue, const char* label, int displayValue);
void updateRTC();
// void displayClock();


#endif