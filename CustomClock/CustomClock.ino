// include the library code:
#include <RTClib.h>
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
RTC_DS1307 rtc;
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
#define ENCODER_CLK 2
#define ENCODER_DT 4
#define ENCODER_BTN 3

int hour = 0;
int minute = 0;
int menu = 0;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  // lcd.print("Hello, World!");
  Serial.begin(9600);

  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_BTN, INPUT_PULLUP);
//  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), readEncoder, FALLING);
//  attachInterrupt(digitalPinToInterrupt(ENCODER_BTN), readBtn, RISING);

  if (!rtc.begin()) {
    Serial.println("Could not find RTC");
    while (1);
  }
  
  if (!rtc.isrunning()) {
    Serial.println("RTC not working!");
    // Set the date and time to the date and time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // You can set the date and time to a specific date / time
    // rtc.adjust(DateTime(2024, 3, 23, 11, 15, 0));
  }
}

void loop() {
  // print the number of seconds since reset:
  // lcd.print(millis() / 1000);

  UpdateLCD();

  if(digitalRead(ENCODER_BTN) == LOW) {
    ChangeTime();
  }


  delay(1000);
}

void ChangeTime() {
  SetHour();
  SetMinute();
  rtc.adjust(DateTime(2024, 3, 23, hour, minute, 0));
}


int SetHour() {
  bool isActive = true;
  Serial.println("Entered set hour func.");

  do{

    if (digitalRead(ENCODER_CLK) == LOW){
      if (hour == 23) {
        hour = 0;
      } else {
        hour++;
      }
    }

    if (digitalRead(ENCODER_DT) == LOW){
      if (hour == 0) {
        hour = 23;
      } else {
        hour--;
      }
    }

    if (digitalRead(ENCODER_BTN) == LOW) {
      Serial.print("Hour is: ");
      Serial.println(hour);
      isActive = false;
    }

    lcd.setCursor(0,0);
    lcd.print("Set time:");
    lcd.setCursor(0,1);
    lcd.print(hour, DEC);
    delay(200);

  } while (isActive);
  
  Serial.println("Exited while loop.");
}


int SetMinute() {
  bool isActive = true;
  Serial.println("Entered set minute func.");

  do{

    if (digitalRead(ENCODER_CLK) == LOW){
      if (minute == 59) {
        minute = 0;
      } else {
        minute++;
      }
    }

    if (digitalRead(ENCODER_DT) == LOW){
      if (minute == 0) {
        minute = 59;
      } else {
        minute--;
      }
    }

    if (digitalRead(ENCODER_BTN) == LOW) {
      Serial.print("Minute is: ");
      Serial.println(minute);
      isActive = false;
    }

    lcd.setCursor(0,0);
    lcd.print("Set time:");
    lcd.setCursor(0,1);
    lcd.print(minute, DEC);
    delay(200);

  } while (isActive);
  
  Serial.println("Exited while loop.");
}


void readBtn() {
  int btn = digitalRead(ENCODER_BTN);

  if (btn == LOW) {
    Serial.println("Button pressed.");
  }
}

void readEncoder() {
  int dtValue = digitalRead(ENCODER_DT);

  if (dtValue == HIGH) {
    Serial.println("Rotated clockwise ⏩");
  }
  if (dtValue == LOW) {
    Serial.println("Rotated counterclockwise ⏪");
  }

}

void UpdateLCD(){
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  DateTime now = rtc.now();
  lcd.setCursor(0, 1);

  int hour;
  String ampm;

  if (now.hour() > 12) {
    hour = now.hour() - 12;
  }

  if (now.hour() >= 12) {
    ampm = "PM";
  } else {
    ampm = "AM";
  }

  if (hour < 10) {
    lcd.print(' ');
  }

  lcd.print(hour);
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
  lcd.print(ampm);
}
