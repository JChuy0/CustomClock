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

bool editModeEnabled = true;
volatile uint64_t lastPulse;
volatile uint64_t lastBMEReading;

#include "AirSensor.h"
#include "Clock.h"
#include "Alarm.h"

BME680Data airData;

struct timeData {
  int hour = 0;
  int minute = 0;
  int second = 0;
  bool isAlarmOn = false;
};

timeData clockTime;
timeData alarmTime;

void TFT_Display(timeData &time, const char* label = "");


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
  if (now - lastPulse >= 500) {
    lastPulse = now;
    menu++;
    editModeEnabled = true;
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


DateTime currTime;
bool temp = false;

void loop() {
  unsigned long now = millis();

  if (menu == 0) {
    // displayClock();
    currTime = getCurrentTime();
    testDisplay();
    // TFT_Display(clockTime);
    checkAlarm();

    // Get a new reading from BME680 every X minutes
    if(now - lastBMEReading >= 5000) {
      lastBMEReading = now;
      airData = readBME680();
    }

  } else if (menu == 1) {
    clockTime.hour = setClockHour();
    TFT_Display(clockTime, "Set Clock Hour");
  } else if (menu == 2) {
    clockTime.minute = setClockMinute();
    TFT_Display(clockTime, "Set Clock Minute");
  } else if (menu == 3) {
    alarmTime.hour = setAlarmHour();
    TFT_Display(alarmTime, "Set Alarm Hour");
  } else if (menu == 4) {
    alarmTime.minute = setAlarmMinute();
    TFT_Display(alarmTime, "Set Alarm Minute");
  } else if (menu == 5) {
    // clockTime.isAlarmOn = turnAlarmOnOff();
    temp = turnAlarmOnOff();
    displayAlarmOnOff();
    // display the result on screen
  } else {
    updateRTC();
    tft.fillScreen(TFT_BLACK);
    menu = 0;
  }
}

void testDisplay() {
  // char timeStr[12];


  // tft.setTextSize(2);

  // tft.setTextColor(TFT_WHITE, TFT_BLACK);
  // tft.drawString("Time is:", 0, 200);
  // tft.drawString(timeStr, 0, 250, 4);


  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // Air Sensor Data
  char temperature[20];
  char humidity[20];

  sprintf(temperature, "%.1f C", airData.temperature);
  sprintf(humidity, "%.1f %", airData.humidity);

  tft.drawString("Temperature:", 0, 0);
  tft.drawString(temperature, 25, 20, 2);

  tft.drawString("Humidity:", 180, 0);
  tft.drawString(humidity, 195, 20, 2);

  // Clock
  char timeStr[10];
  sprintf(timeStr, "%02d:%02d:%02d", currTime.hour(), currTime.minute(), currTime.second());

  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  // tft.drawString(label, 0, 200);
  tft.drawString(timeStr, 0, 250, 4);

  // Alarm
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Alarm is:", 250, 200, 2);

  if (temp) {
    tft.drawString("On", 250, 250, 2);
  } else {
    tft.drawString("Off", 250, 250, 2);
  }
}



// this should return a boolean
void displayAlarmOnOff() {
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Alarm is:", 250, 200, 2);

  // Serial.print("TurnAlarmOnOff is: ");
  // Serial.println(temp);
  // Serial.println((temp == 0) ? "false" : "true");

  if (temp) {
    tft.drawString("On", 250, 250, 2);
  } else {
    tft.drawString("Off", 250, 250, 2);
  }
}


// BME680Data &value,
// add parameters for air data, time/alarm, and optional parameter for string
void TFT_Display(timeData &time, const char* label) {
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // Air Sensor Data
  char temperature[20];
  char humidity[20];

  sprintf(temperature, "%.1f C", airData.temperature);
  sprintf(humidity, "%.1f %", airData.humidity);

  tft.drawString("Temperature:", 0, 0);
  tft.drawString(temperature, 25, 20, 2);

  tft.drawString("Humidity:", 180, 0);
  tft.drawString(humidity, 195, 20, 2);

  // Clock
  char timeStr[9];
  sprintf(timeStr, "%02d:%02d", time.hour, time.minute);

  tft.setTextSize(2);
  // tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(label, 0, 200);
  tft.drawString(timeStr, 0, 250, 4);

  // Alarm
  // tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Alarm is:", 250, 200, 2);

  if (temp) {
    tft.drawString("On", 250, 250, 2);
  } else {
    tft.drawString("Off", 250, 250, 2);
  }
}


void clearScreen() {
  if (editModeEnabled) {
    editModeEnabled = false;
    tft.fillScreen(TFT_BLACK);
  }
}
