/*
  I'm building my own alarm clock so I can customize both the functionality and the appearance.
  Author: JC
  Date started: December 13, 2025

  Brief summary:
    An encoder button navigates through the menu, and rotating the knob adjusts the value.

  Thanks to the Arduino Discord server for their advice and assistance.

*/

// Libraries
#include "Wire.h"
#include "SPI.h"
#include "TFT_eSPI.h"
#include "RTClib.h"
#include "Adafruit_BME680.h"

// Initialize the pins
TFT_eSPI tft = TFT_eSPI();
RTC_DS1307 rtc;
Adafruit_BME680 bme; // I2C
#define ENCODER_CLK 25
#define ENCODER_DT 26
#define ENCODER_BTN 27

// Variables
int updateClockHour = 0;
int updateClockMinute = 0;
int clockSecond = 0;
int alarmHour = 0;
int alarmMinute = 0;
int menu = 0;
int encoderValue = 0;

bool isActive = false;
bool isAlarmOn = false;
bool isAlarmRinging = false;
volatile uint64_t lastPulse;
volatile uint64_t lastBMEReading;

// Set up the notes and melody
#define NOTE_C4  262
#define NOTE_G3  196

int noteDurations[] = {
  4, 8, 4, 8, 4, 8, 4, 8
};

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_C4, NOTE_G3, NOTE_C4, NOTE_G3, NOTE_C4, NOTE_G3
};


void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println(F("BME680 async test"));

  tft.init();
  Wire.begin(21, 22);

  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), readEncoder, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_BTN), readButton, FALLING);

  isAlarmOn = true;

  if (!rtc.begin()) {
    Serial.println("Could not find RTC");
    while (1);
  }

  if (!rtc.isrunning()) {
    // Sets time to January 1st, 2025
    rtc.adjust(DateTime(2025, 1, 1, 1, 0, 0));
  }

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setRotation(3);

  if (!bme.begin()) {
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
    while (1);
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);

  testAlarm();
}


void testAlarm() {
  // year month day hour minute second
  rtc.adjust(DateTime(2025, 1, 1, 18, 44, 40));

  alarmHour = 17;
  alarmMinute = 45;
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
  unsigned long now = millis();

  if (menu == 0) {
    DisplayClock();
    CheckAlarm();

    // Get a new reading from BME680 every X minutes
    if(now - lastBMEReading >= 5000) {
      lastBMEReading = now;
      ReadBME680();
    }

  } else if (menu == 1) {
    GetCurrentTime();
    SetTime(updateClockHour, 23, "Set Clock Hour", updateClockMinute);
  } else if (menu == 2) {
    SetTime(updateClockMinute, 59, "Set Clock Minute", updateClockHour);
  } else if (menu == 3) {
    ClearScreen();
    SetTime(alarmHour, 23, "Set Alarm Hour", alarmMinute);
  } else if (menu == 4) {
    SetTime(alarmMinute, 59, "Set Alarm Minute", alarmHour);
  } else if (menu == 5) {
    TurnAlarmOnOff();
  } else {
    UpdateRTC();
    menu = 0;
  }

}


void GetCurrentTime() {
  if (isActive) {
    DateTime now = rtc.now(); 
    updateClockHour = now.hour();
    updateClockMinute = now.minute();
    isActive = false;
    tft.fillScreen(TFT_BLACK);
  }
}

void ClearScreen() {
  if (isActive) {
    isActive = false;
    tft.fillScreen(TFT_BLACK);
  }
}

// A generic function to set any time value (hours or minutes)
// editingValue: The value being adjusted (hour or minute).
// maxValue: Upper limit. 23 for hour, 59 for minute.
// label: Screen label (e.g. "Set alarm hour").
// displayValue: The paired time value shown for context .
void SetTime(int &editingValue, int maxValue, const char* label, int displayValue) {
  editingValue += encoderValue;
  encoderValue = 0;

  if (editingValue < 0) {
    editingValue = maxValue;
  } else if (editingValue > maxValue) {
    editingValue = 0;
  }

  tft.setTextSize(2);
  char timeStr[9];

  if (maxValue == 23) {
    sprintf(timeStr, "%02d:%02d", editingValue, displayValue);
  } else if (maxValue == 59) {
    sprintf(timeStr, "%02d:%02d", displayValue, editingValue);
  }

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(label, 0, 200);
  tft.drawString(timeStr, 0, 250, 4);
}


// Reads the temperature and humidity from a BME680 sensor
void ReadBME680() {
  char temperature[20];
  char humidity[20];
  unsigned long endTime = bme.beginReading();

  // Get measurement results from BME680.
    if (!bme.endReading()) {
    Serial.println(F("Failed to complete reading :("));
    return;
  }

  sprintf(temperature, "%.1f C", bme.temperature);
  sprintf(humidity, "%.1f %", bme.humidity);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Temperature:", 0, 0);
  tft.drawString(temperature, 25, 20, 2);

  tft.drawString("Humidity:", 180, 0);
  tft.drawString(humidity, 195, 20, 2);
}

// Updates the time on the clock module
void UpdateRTC() {
  rtc.adjust(DateTime(2025, 1, 1, updateClockHour, updateClockMinute, clockSecond));
  tft.fillScreen(TFT_BLACK);
}


void CheckAlarm() {
  DateTime now = rtc.now();

  // I coded things this way so the alarm will continue to ring until it is turned off.
  if (isAlarmOn && (now.hour() == alarmHour) && (now.minute() == alarmMinute) && (now.second() == 0) ) {
    isAlarmRinging = true;
  }

  if (isAlarmRinging) {
    AlarmBuzzer();
  }
}

void AlarmBuzzer() {
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];

    // pin number, sound to play, duration
    tone(33, melody[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(33);
  }
}


void TurnAlarmOnOff() {
  if (encoderValue != 0) {
    isAlarmOn = !isAlarmOn;
    encoderValue = 0;
  }

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Alarm is:", 0, 0, 2);

  if (isAlarmOn) {
    tft.drawString("On", 0, 100, 2);
  } else {
    tft.drawString("Off", 0, 100, 2);
  }
}

void DisplayClock(){
  DateTime now = rtc.now();

  tft.setTextSize(2);

  char timeStr[9];
  sprintf(timeStr, "%2d:%02d:%02d", now.hour(), now.minute(), now.second());

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(timeStr, 0, 200, 4);

  // Display if alarm is on or off
  if (isAlarmOn) {
    tft.drawString("On", 300, 200, 4);
  } else {
    tft.drawString("Off", 300, 200, 4);
  }
}
