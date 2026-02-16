#ifndef AIR_SENSOR_H
#define AIR_SENSOR_H

#include "Data_Types.h"
// #include "TFT_eSPI.h"
// extern TFT_eSPI tft;

// struct BME680Data {
//   float temperature;
//   float humidity;
//   // float pressure; 
//   // float gasResistance;
// };

void setupBME680();
BME680Data readBME680();

#endif