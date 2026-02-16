#ifndef DATA_TYPES_H
#define DATA_TYPES_H

struct BME680Data {
  float temperature;
  float humidity;
  // float pressure; 
  // float gasResistance;
};

struct timeData {
  int hour = 0;
  int minute = 0;
  int second = 0;
  // bool isAlarmOn = false;
};

#endif