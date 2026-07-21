/*
  I'm building my own alarm clock so I can customize both the functionality and the appearance.
  Author: JC
  Date started: December 13, 2025

  Brief summary:
    An encoder button navigates through the menu, and rotating the knob adjusts the value.

    Thanks to the Arduino Discord server for their advice and assistance. And to RandomNerdTutorials for their excellent tutorials.
*/

// Libraries
#include <Arduino.h>
#include <Wire.h> // Enables I2C communication
#include <SPI.h>  // Enables SPI communication
#include "Data_Types.h"
#include "Display.h"

// Initialize the pins
#define ENCODER_CLK 26
#define ENCODER_DT 27
#define ENCODER_BTN 14


// Variables
int menu = 0;
int encoderValue = 0;
bool alarmOnOffValue = true;
bool isAlarmRinging = false;

volatile uint64_t lastPulse;
volatile uint64_t lastBMEReading;
volatile uint64_t lastImageUpdate;
volatile uint64_t lastClockUpdate;

#include "AirSensor.h"
#include "Clock.h"
#include "Alarm.h"

BME680Data airData;
timeData clockTime;
timeData alarmTime;


void setup() {
  Serial.begin(9600);
  delay(3000);   // add a brief pause so the serial monitor can start up

  Wire.begin(21, 22);
  setupScreen();
  setupRTC();
  setupBME680();
  alarmSetup();

  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), readEncoder, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_BTN), readButton, FALLING);

  testAlarm();
  loadAnimation("/clock/");
}


void readButton() {
  unsigned long now = millis();
  encoderValue = 0;

  // Debounce: filter out button noise, ensure code is only run once per press
  if (now - lastPulse >= 500) {
    lastPulse = now;
    menu++;
  }

  // See if I can find a way to decouple isAlarmRinging

  // Turns alarm off when button is pressed
  if (isAlarmRinging) {
    isAlarmRinging = false;
    menu = 0;
  }
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
  handleMenu();
}



void handleMenu() {
  unsigned long now = millis();

  switch(menu) {
    case 0:
      // Update BME680 every X milliseconds
      if(now - lastBMEReading >= 60000) {    // currently get update every 60 seconds          <===  CHANGE THIS WHEN DONE TESTING
        lastBMEReading = now;
        airData = readBME680();
        getCurrentTime();
      }

      // Update the image every X milliseconds
      if (isAlarmRinging == false) {
        if (now - lastImageUpdate >= 100) {
          lastImageUpdate = now;
          displayImage();
        }
      }

      // Update the displayed time every X milliseconds
      if (now - lastClockUpdate >= 1000) {
        lastClockUpdate = now;
        displayClock(airData, alarmOnOffValue);
      }

      checkAlarm();
      break;
    case 1: 
      clockTime.hour = setClockHour();
      displayTimeEditor(clockTime, "Set Clock Hour");
      break;
    case 2: 
      clockTime.minute = setClockMinute();
      displayTimeEditor(clockTime, "Set Clock Minute");
      break;
    case 3: 
      alarmTime.hour = setAlarmHour();
      displayTimeEditor(alarmTime, "Set Alarm Hour  ");
      break;
    case 4: 
      alarmTime.minute = setAlarmMinute();
      displayTimeEditor(alarmTime, "Set Alarm Minute");
      break;
    case 5: 
      alarmOnOffValue = turnAlarmOnOff();
      displayAlarmOnOff(alarmOnOffValue, "Alarm is: ");  // This will display a label while in edit mode, but not in display mode
      break;
    default: 
      updateRTC();
      resetScreen();
      menu = 0;
  }
}
