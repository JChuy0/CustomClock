#ifndef AIR_SENSOR_H
#define AIR_SENSOR_H

#include "TFT_eSPI.h"
extern TFT_eSPI tft;

void setupBME680();
void readBME680();

#endif