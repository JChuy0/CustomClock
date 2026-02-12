/*
  I'm building my own alarm clock so I can customize both the functionality and the appearance.
  Author: JC
  Date started: December 13, 2025

  Brief summary:
    An encoder button navigates through the menu, and rotating the knob adjusts the value.

    Thanks to the Arduino Discord server for their advice and assistance. And to RandomNerdTutorials for their excellent tutorials.
*/

// Libraries
#include "Wire.h"
#include "SPI.h"
#include "TFT_eSPI.h"


// Initialize the pins
TFT_eSPI tft = TFT_eSPI();

#define ENCODER_CLK 25
#define ENCODER_DT 26
#define ENCODER_BTN 27


// Variables

int menu = 0;
int encoderValue = 0;

bool editModeInitialized = true;
volatile uint64_t lastPulse;
volatile uint64_t lastBMEReading;


// int updateClockHour = 0;
// int updateClockMinute = 0;
// int clockSecond = 0;
// int alarmHour = 0;
// int alarmMinute = 0;

#include "AirSensor.h"
#include "Clock.h"
#include "Alarm.h"

BME680Data airData;




struct timeData {
  int clockHour;
  int clockMinute;
  int alarmHour;
  int alarmMinute;
};

timeData currTime;


void setup() {
  Serial.begin(9600);
  while(!Serial);   // add a brief pause so the serial monitor can start up

  tft.init();
  Wire.begin(21, 22);

  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), readEncoder, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_BTN), readButton, FALLING);

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setRotation(3);

  setupRTC();
  setupBME680();

  // testAlarm();
}


void readButton() {
  unsigned long now = millis();
  encoderValue = 0;

  // Debounce: filter out button noise, ensure code is only run once per press
  if (now - lastPulse >= 1000) {
    lastPulse = now;
    menu++;
    editModeInitialized = false;
  }

  // Turns alarm off when button is pressed
  // if (isAlarmRinging) {
  //   isAlarmRinging = false;
  //   menu = 0;
  // }
}


void readEncoder() {
  unsigned long now = millis();

  // Debounce: filter out encoder noise, ensure code is only run once per twist
  if(now - lastPulse >= 500) {
    lastPulse = now;

    // Check rotation direction based on DT pin state
    if (digitalRead(ENCODER_DT)) {
      encoderValue++;
    } else {
      encoderValue--;
    }
  }
}

void loop() {
  unsigned long now = millis();

  if (menu == 0) {
    // displayClock();
    checkAlarm();

    // Get a new reading from BME680 every X minutes
    if(now - lastBMEReading >= 5000) {
      lastBMEReading = now;
      airData = readBME680();
    }

  } else if (menu == 1) {
    currTime.clockHour = setClockHour();
    // getCurrentTime();
    // setTime(updateClockHour, 23, "Set Clock Hour", updateClockMinute);
  } else if (menu == 2) {
    currTime.clockMinute = setClockMinute();
    // setTime(updateClockMinute, 59, "Set Clock Minute", updateClockHour);
  } else if (menu == 3) {
    // timeTest.alarmHour = setAlarmHour();
    // clearScreen();
    // setTime(alarmHour, 23, "Set Alarm Hour", alarmMinute);
  } else if (menu == 4) {
    // timeTest.alarmMinute = setAlarmMinute();
    // setTime(alarmMinute, 59, "Set Alarm Minute", alarmHour);
  } else if (menu == 5) {
    turnAlarmOnOff();
  } else {
    updateRTC();
    menu = 0;
  }

  testDisplay();
}

void testDisplay() {
  char temperature[20];
  char humidity[20];

  sprintf(temperature, "%.1f C", airData.temperature);
  sprintf(humidity, "%.1f %", airData.humidity);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Temperature:", 0, 0);
  tft.drawString(temperature, 25, 20, 2);

  tft.drawString("Humidity:", 180, 0);
  tft.drawString(humidity, 195, 20, 2);

  tft.setTextSize(2);
  char timeStr[9];

  sprintf(timeStr, "%02d:%02d", currTime.clockHour, currTime.clockMinute);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Set time", 0, 200);
  tft.drawString(timeStr, 0, 250, 4);
}


// void clearScreen() {
//   if (isActive) {
//     isActive = false;
//     tft.fillScreen(TFT_BLACK);
//   }
// }
