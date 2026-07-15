#ifndef DISPLAY_H
#define DISPLAY_H

#include <FS.h>
#include <TFT_eSPI.h>
#include <RTClib.h>
#include <LittleFS.h>
#include <PNGdec.h>
#include "Data_Types.h"

void setupScreen();
void resetScreen();
void displayClock(DateTime currTime, BME680Data airData, bool alarmOnOffValue);
void displayTimeEditor(timeData &time, const char* label);
void displayAlarmOnOff(bool alarmOnOff, const char* label);

void loadAnimation(const char* label);
void displayImage();

#endif