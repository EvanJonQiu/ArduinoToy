#include <Wire.h>
#include <rgb_lcd.h>
#include <RTClib.h>
#include <EasyButton.h>

rgb_lcd lcd;

const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sun","Mon", "Tues", "Wed", "Thu", "Fri", "Sat"};

const int input1Pin = 2;
const int input2Pin = 4;
const int input3Pin = 5;
const int input4Pin = 6;

EasyButton button1(input1Pin);
EasyButton button2(input2Pin);
EasyButton button3(input3Pin);
EasyButton button4(input4Pin);

volatile int mode = 1;
volatile int setMode = 1;

int row = 0;
int col = 0;

DateTime now;
DateTime newDateTime;

const uint8_t daysInMonth[] = {31, 28, 31, 30, 31, 30,
                               31, 31, 30, 31, 30, 31};

void onButton1Pressed() {
  if (mode == 1) {
    mode = 2;
    lcd.blink();
  } else {
    mode = 1;
    setMode = 1;
    col = 0;
    row = 0;
    lcd.noBlink();
    rtc.adjust(now);
  }
}

void onButton2Pressed() {
  if (mode == 2) {
    int year = now.year();
    int month = now.month();
    int day = now.day();
    int hour = now.hour();
    int minute = now.minute();
    int second = now.second();
    if (setMode == 2) {
      year = year == 2000 ? 2099 : year - 1;
    } else if (setMode == 3) {
      month = month == 1 ? 12 : month - 1;
      int newMonth = daysInMonth[month - 1];
      if (day > newMonth) {
        day = newMonth;
      }
    } else if (setMode == 4) {
      int newMonth = daysInMonth[month - 1];
      day = day == 1 ? newMonth : day - 1;
    } else if (setMode == 5) {
      hour = hour == 0 ? 23 : hour - 1;
    } else if (setMode == 6) {
      minute = minute == 0 ? 59 : minute - 1;
    } else if (setMode == 7) {
      second = second == 0 ? 59 : second - 1;
    }
    now = DateTime(year, month, day, hour, minute, second);
  }
}

void onButton3Pressed() {
  if (mode == 2) {
    int year = now.year();
    int month = now.month();
    int day = now.day();
    int hour = now.hour();
    int minute = now.minute();
    int second = now.second();
    if (setMode == 2) {
      year = year == 2099 ? 2000 : year + 1;
    } else if (setMode == 3) {
      month = month == 12 ? 1 : month + 1;
      int newMonth = daysInMonth[month - 1];
      if (day > newMonth) {
        day = newMonth;
      }
    } else if (setMode == 4) {
      int newMonth = daysInMonth[month - 1];
      day = day == newMonth ? 1 : day + 1;
    } else if (setMode == 5) {
      hour = hour == 23 ? 0 : hour + 1;
    } else if (setMode == 6) {
      minute = minute == 59 ? 0 : minute + 1;
    } else if (setMode == 7) {
      second = second == 59 ? 0 : second + 1;;
    }
    now = DateTime(year, month, day, hour, minute, second);
  }
}

void onButton4Pressed() {
  if (mode == 2) {
    if (setMode == 1) {
      col = 3;
      setMode++;
    } else if (setMode == 2) {
      col = 6;
      setMode++;
    } else if (setMode == 3) {
      col = 9;
      setMode ++;
    } else if (setMode == 4) {
      row = 1;
      col = 1;
      setMode++;
    } else if (setMode == 5) {
      col = 4;
      setMode++;
    } else if (setMode == 6) {
      col = 7;
      setMode++;
    } else if (setMode == 7) {
      row = 0;
      col = 3;
      setMode = 2;
    }
  }
}

void setup() {
  button1.begin();
  button2.begin();
  button3.begin();
  button4.begin();

  button1.onPressed(onButton1Pressed);
  button2.onPressed(onButton2Pressed);
  button3.onPressed(onButton3Pressed);
  button4.onPressed(onButton4Pressed);

  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);
  //lcd.blink();

  Serial.begin(9600);
  if (!rtc.begin()) {
    Serial.println("Couldn't findRTC");
    while (1);
  }
  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(2021, 7, 6, 23, 17, 0));
  }
}

void loop() {

  button1.read();

  if (mode == 1) {
    now = rtc.now();

    lcd.setCursor(0, 0);
    char strDate[32] = {0};
    //sprintf(strDate, "%02d/%02d/%02d %s", now.day(), now.month(), now.year(), daysOfTheWeek[now.dayOfTheWeek()]);
    sprintf(strDate, "%02d/%02d/%02d", now.year(), now.month(), now.day());
    lcd.print(strDate);

    lcd.setCursor(0, 1);
    char strTime[32] = {0};
    sprintf(strTime, "%02d:%02d:%02d",  now.hour(), now.minute(), now.second());
    lcd.print(strTime);
  } else {
    lcd.setCursor(0, 0);
    char strDate[32] = {0};
    //sprintf(strDate, "%02d/%02d/%02d %s", now.day(), now.month(), now.year(), daysOfTheWeek[now.dayOfTheWeek()]);
    sprintf(strDate, "%02d/%02d/%02d", now.year(), now.month(), now.day());
    lcd.print(strDate);

    lcd.setCursor(0, 1);
    char strTime[32] = {0};
    sprintf(strTime, "%02d:%02d:%02d",  now.hour(), now.minute(), now.second());
    lcd.print(strTime);

    newDateTime = now;
    lcd.setCursor(col, row);
    button2.read();
    button3.read();
    button4.read();
  }
  
  delay(20);
}

void printNow() {
  Serial.print(now.day(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.year(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
}