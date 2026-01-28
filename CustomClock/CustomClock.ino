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

// Initialize the pins
TFT_eSPI tft = TFT_eSPI();
RTC_DS1307 rtc;
#define ENCODER_CLK 25
#define ENCODER_DT 26
#define ENCODER_BTN 27

// Variables
int updateClockHour = 0;
int updateClockMinute = 0;
int clockSecond = 0;
int alarmHour = 13;
int alarmMinute = 12;

int menu = 0;
int num = 0;
bool isActive = false;
bool isAlarmOn = false;
bool isAlarmRinging = false;
volatile uint64_t lastPulse;

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

  Serial.println("Test 1");

  // rtc.adjust(DateTime(2025, 12, 28, 13, 40, 0));

  if (!rtc.isrunning()) {
    // Sets time to January 1st, 2025
    rtc.adjust(DateTime(2025, 1, 1, 1, 0, 0));
  }

  Serial.println("Test 2");

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setRotation(3);
}

void readButton() {
  uint64_t now = millis();
  num = 0;

  if ((now - lastPulse >= 1000)) {
    lastPulse = now;
    menu++;
    isActive = true;
  }

  if (isAlarmRinging) {
    isAlarmRinging = false;
    menu = 0;
  }

}


void readEncoder() {
  uint64_t now = millis();

  if(now - lastPulse >= 500) {
    lastPulse = now;

    if (digitalRead(ENCODER_DT)) {
      num++;
    } else {
      num--;
    }
  }

}

void loop() {

  if (menu == 0) {
    DisplayClock();
    CheckAlarm();
  } else if (menu == 1) {
    DisplaySetHour();
  } else if (menu == 2) {
    DisplaySetMinute();
  } else if (menu == 3) {
    SetAlarmHour();
  } else if (menu == 4) {
    SetAlarmMinute();
  } else if (menu == 5) {
    TurnAlarmOnOff();
  } else {
    UpdateRTC();
    menu = 0;
  }

  delay(1000);

}


void UpdateRTC() {
  rtc.adjust(DateTime(2025, 12, 22, updateClockHour, updateClockMinute, clockSecond));
  tft.fillScreen(TFT_BLACK);
}

void CheckAlarm() {
  DateTime now = rtc.now();

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
    tone(5, melody[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(5);
  }
}

void TurnAlarmOnOff() {
  tft.fillScreen(TFT_BLACK);

  if (num != 0) {
    isAlarmOn = !isAlarmOn;
    num = 0;
  }

  tft.drawString("turn alarm on/off", 0, 0, 2);


  // lcd.setCursor(0,0);
  // lcd.print("Alarm is:");
  // lcd.setCursor(0,1);

  if (isAlarmOn) {
    // lcd.print("On");
  } else {
    // lcd.print("Off");
  }

  delay(200);
}

void DisplaySetHour() {
  tft.fillScreen(TFT_BLACK);

  // I created a datatime object and grabbed all the data I needed from it to avoid creating a new datatime object in each subsequent method.
  if (isActive) {
    DateTime now = rtc.now(); 
    updateClockHour = now.hour();
    updateClockMinute = now.minute();
    isActive = false;
  }

  updateClockHour += num;
  num = 0;

  if (updateClockHour > 23) {
    updateClockHour = 0;
  } else if (updateClockHour < 0) {
    updateClockHour = 23;
  }



  // set text size and color
  // set cursor position
  // print text

  // tft.setTextSize(5);
  // tft.setTextColor(TFT_GREEN, TFT_BLACK);   // Set text color to green and padding to back
  // tft.setCursor(0, 15);

  // tft.print("Set hour:");
  // tft.setCursor(0,100);
  // tft.print(updateClockHour);

  delay(200);
}

void DisplaySetMinute() {
  tft.fillScreen(TFT_BLACK);

  updateClockMinute += num;
  num = 0;

  if (updateClockMinute > 59) {
    updateClockMinute = 0;
  } else if (updateClockMinute < 0) {
    updateClockMinute = 59;
  }

  char buffer[7];
  itoa(updateClockMinute, buffer, 10);

  tft.drawString("set clock minute", 0, 0, 2);
  tft.drawString(buffer, 0, 100, 2);

  // tft.setTextSize(5);
  // tft.setTextColor(TFT_GREEN, TFT_BLACK);   // Set text color to green and padding to back
  // tft.setCursor(0, 15);

  // tft.print("Set minute:");
  // tft.setCursor(0,100);
  // tft.print(updateClockMinute);

  delay(200);
}


void SetAlarmHour() {
  tft.fillScreen(TFT_BLACK);

  alarmHour += num;
  num = 0;

  if (alarmHour > 23) {
    alarmHour = 0;
  } else if (alarmHour < 0) {
    alarmHour = 23;
  }

  tft.drawString("set alarm hour", 0, 0, 2);

  // lcd.setCursor(0,0);
  // lcd.print("Set alarm hour:");
  // lcd.setCursor(0,1);
  // lcd.print(alarmHour);
  delay(200);
}

void SetAlarmMinute() {
  tft.fillScreen(TFT_BLACK);

  alarmMinute += num;
  num = 0;

  if (alarmMinute > 59) {
    alarmMinute = 0;
  } else if (alarmMinute < 0) {
    alarmMinute = 59;
  }

  tft.drawString("set alarm minute", 0, 0, 2);

  // lcd.setCursor(0,0);
  // lcd.print("Set alarm minute:");
  // lcd.setCursor(0,1);
  // lcd.print(alarmMinute);
  delay(200);
}


void DisplayClock(){
  DateTime now = rtc.now();

  tft.setTextSize(2);

  char timeStr[9];
  sprintf(timeStr, "%2d:%02d:%02d", now.hour(), now.minute(), now.second());

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(timeStr, 0, 0, 4);

  // // Display if alarm is on or off
  if (isAlarmOn) {
    tft.drawString("On", 100, 200, 4);
  } else {
    tft.drawString("Off", 100, 200, 4);
  }

}
