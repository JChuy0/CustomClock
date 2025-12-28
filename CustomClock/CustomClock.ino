/*
  I'm building my own alarm clock so I can customize both the functionality and the appearance.
  Author: JC
  Date started: December 13, 2025

  Brief summary:
    The encoder button navigates the menu, and rotation adjusts the value.

  Thanks to the Arduino Discord server for advice and assistance.
*/

// include the library code:
#include <RTClib.h>
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
RTC_DS1307 rtc;
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
#define ENCODER_CLK 2
#define ENCODER_DT 4
#define ENCODER_BTN 3

// Variables
int updateHour = 0;
int updateMinute = 0;
int menu = 0;
int num = 0;
bool isActive = false;
volatile uint64_t lastPulse;

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);

  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), readEncoder, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_BTN), readBtn, FALLING);

  if (!rtc.begin()) {
    Serial.println("Could not find RTC");
    while (1);
  }

  // rtc.adjust(DateTime(2025, 12, 22, 20, 25, 0));

  if (!rtc.isrunning()) {
    // Sets time to January 1st, 2025
    rtc.adjust(DateTime(2025, 1, 1, 1, 0, 0));
  }
}

void readBtn() {
  uint64_t now = millis();
  num = 0;

  if (now - lastPulse >= 2000) {
    menu++;
    isActive = true;
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
  } else if (menu == 1) {
    DisplaySetHour();
  } else if (menu == 2) {
    DisplaySetMinute();
  } else {
    UpdateRTC();
    menu = 0;
  } 
}

void UpdateRTC() {
  rtc.adjust(DateTime(2025, 12, 22, updateHour, updateMinute, 0));
  lcd.clear();
}

void DisplaySetHour() {
  lcd.clear();

  // I created a datatime object and grabbed all the data I needed from it to avoid creating a new datatime object in each subsequent method.
  if (isActive) {
    DateTime now = rtc.now(); 
    updateHour = now.hour();
    updateMinute = now.minute();
    isActive = false;
  }

  updateHour += num;
  num = 0;

  if (updateHour > 23) {
    updateHour = 0;
  } else if (updateHour < 0) {
    updateHour = 23;
  }

  lcd.setCursor(0,0);
  lcd.print("Set hour:");
  lcd.setCursor(0,1);
  lcd.print(updateHour);
  delay(200);
}

void DisplaySetMinute() {
  lcd.clear();

  updateMinute += num;
  num = 0;

  if (updateMinute > 59) {
    updateMinute = 0;
  } else if (updateMinute < 0) {
    updateMinute = 59;
  }

  lcd.setCursor(0,0);
  lcd.print("Set minute:");
  lcd.setCursor(0,1);
  lcd.print(updateMinute);
  delay(200);
}


void DisplayClock(){
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  DateTime now = rtc.now();
  lcd.setCursor(0, 1);

  if (now.hour() < 10) {
    lcd.print(' ');
  }

  lcd.print(now.hour());
  lcd.print(':');

  if (now.minute() < 10) {
    lcd.print(0);
  }

  lcd.print(now.minute());
  lcd.print(':');

  if (now.second() < 10) {
    lcd.print(0);
  }

  lcd.print(now.second());
}
