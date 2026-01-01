/*
  I'm building my own alarm clock so I can customize both the functionality and the appearance.
  Author: JC
  Date started: December 13, 2025

  Brief summary:
    The encoder button navigates the menu, and rotation adjusts the value.

  Thanks to the Arduino Discord server for advice and assistance.
*/

// Libraries
#include <RTClib.h>
#include <LiquidCrystal.h>

// Initialize the pins
RTC_DS1307 rtc;
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
#define ENCODER_CLK 2
#define ENCODER_DT 4
#define ENCODER_BTN 3

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
  lcd.begin(16, 2);
  Serial.begin(9600);

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

  // rtc.adjust(DateTime(2025, 12, 28, 13, 40, 0));

  if (!rtc.isrunning()) {
    // Sets time to January 1st, 2025
    rtc.adjust(DateTime(2025, 1, 1, 1, 0, 0));
  }
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
}

void UpdateRTC() {
  rtc.adjust(DateTime(2025, 12, 22, updateClockHour, updateClockMinute, clockSecond));
  lcd.clear();
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
  lcd.clear();

  if (num != 0) {
    isAlarmOn = !isAlarmOn;
    num = 0;
  }

  lcd.setCursor(0,0);
  lcd.print("Alarm is:");
  lcd.setCursor(0,1);

  if (isAlarmOn) {
    lcd.print("On");
  } else {
    lcd.print("Off");
  }

  delay(200);
}

void DisplaySetHour() {
  lcd.clear();

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

  lcd.setCursor(0,0);
  lcd.print("Set hour:");
  lcd.setCursor(0,1);
  lcd.print(updateClockHour);
  delay(200);
}

void DisplaySetMinute() {
  lcd.clear();

  updateClockMinute += num;
  num = 0;

  if (updateClockMinute > 59) {
    updateClockMinute = 0;
  } else if (updateClockMinute < 0) {
    updateClockMinute = 59;
  }

  lcd.setCursor(0,0);
  lcd.print("Set minute:");
  lcd.setCursor(0,1);
  lcd.print(updateClockMinute);
  delay(200);
}


void SetAlarmHour() {
  lcd.clear();

  alarmHour += num;
  num = 0;

  if (alarmHour > 23) {
    alarmHour = 0;
  } else if (alarmHour < 0) {
    alarmHour = 23;
  }

  lcd.setCursor(0,0);
  lcd.print("Set alarm hour:");
  lcd.setCursor(0,1);
  lcd.print(alarmHour);
  delay(200);
}

void SetAlarmMinute() {
  lcd.clear();

  alarmMinute += num;
  num = 0;

  if (alarmMinute > 59) {
    alarmMinute = 0;
  } else if (alarmMinute < 0) {
    alarmMinute = 59;
  }

  lcd.setCursor(0,0);
  lcd.print("Set alarm minute:");
  lcd.setCursor(0,1);
  lcd.print(alarmMinute);
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

  // display alarm timer
  lcd.setCursor(11, 1);

  lcd.print(alarmHour);
  lcd.print(':');
  lcd.print(alarmMinute);

  // Display if alarm is on or off
  lcd.setCursor(11, 0);

  if (isAlarmOn) {
    lcd.print("On");
  } else {
    lcd.print("Off");
  }

}
