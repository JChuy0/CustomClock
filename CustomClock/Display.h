#ifndef DISPLAY_H
#define DISPLAY_H

#include "Data_Types.h"
#include "TFT_eSPI.h"
#include "RTClib.h"

void setup_TFT_Screen();
void resetScreen();
void displayClock(DateTime currTime, BME680Data airData, bool alarmOnOffValue);
void displayTimeEditor(timeData &time, const char* label);
void displayAlarmOnOff(bool alarmOnOff, const char* label);

#endif