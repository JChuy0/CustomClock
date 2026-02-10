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

bool isActive = false;
volatile uint64_t lastPulse;
volatile uint64_t lastBMEReading;


int updateClockHour = 0;
int updateClockMinute = 0;
int clockSecond = 0;
int alarmHour = 0;
int alarmMinute = 0;


#include "ClockAndAlarm.h"
#include "AirSensor.h"

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
    isActive = true;
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
    displayClock();
    checkAlarm();

    // Get a new reading from BME680 every X minutes
    if(now - lastBMEReading >= 5000) {
      lastBMEReading = now;
      readBME680();
    }

  } else if (menu == 1) {
    // getCurrentTime();
    setTime(updateClockHour, 23, "Set Clock Hour", updateClockMinute);
  } else if (menu == 2) {
    setTime(updateClockMinute, 59, "Set Clock Minute", updateClockHour);
  } else if (menu == 3) {
    clearScreen();
    setTime(alarmHour, 23, "Set Alarm Hour", alarmMinute);
  } else if (menu == 4) {
    setTime(alarmMinute, 59, "Set Alarm Minute", alarmHour);
  } else if (menu == 5) {
    turnAlarmOnOff();
  } else {
    updateRTC();
    menu = 0;
  }

}


void clearScreen() {
  if (isActive) {
    isActive = false;
    tft.fillScreen(TFT_BLACK);
  }
}
