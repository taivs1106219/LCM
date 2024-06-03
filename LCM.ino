#include <LiquidCrystal_I2C.h>
#define joystick_sw 0
#define joystick_dx A13
#define joystick_dy A14

LiquidCrystal_I2C lcd(0x27, 16, 2);

int year = 2024, month = 6, day = 3;
int hh = 9, mm = 58, ss = 20;
int dx = 0, dy = 0, enter = 0;
bool doubleDot = false;
bool edit_mode = false;
byte location[] = { 3, 6, 9, 17, 20 };
byte location_index = 0;
int joystick_delay = 300;
unsigned long time0 = 0, time1 = 0;

void setup() {
  _init();
  lcd_init();
  disp_init();
}

void loop() {
  Date_Time_Disp();
  delay(200);

  Serial.print("edit_mode == ");
  Serial.print(edit_mode);
  Serial.print("\tenter==");
  Serial.print(enter);
  Serial.print(dx);
  Serial.print(" ");
  Serial.print(dy);
  Serial.println(" ");


  dx = analogRead(joystick_dx);
  dy = analogRead(joystick_dy);
  enter = digitalRead(joystick_sw);

  if (millis() - time1 > 100) {
    time1 = millis();
    if (enter == 0) {
      edit_mode = !edit_mode;
    }
  }

  if (edit_mode) {
    if (dy < 10) {
      if (location_index == 0) {
        year++;
      } else if (location_index == 1) {
        if (month < 12) {
          month++;
        }
      } else if (location_index == 2) {
        switch (month) {
          case 1:
          case 3:
          case 5:
          case 7:
          case 8:
          case 10:
          case 12:
            if (day < 31) {
              day++;
            }
            break;
          case 4:
          case 6:
          case 9:
          case 11:
            if (day < 30) {
              day++;
            }
            break;
          case 2:
            if (day < 28) {
              day++;
            }
            break;
        }
      } else if (location_index == 3) {
        if (hh < 23) {
          hh++;
        }
      } else if (location_index == 4) {
        if (mm < 59) {
          mm++;
        }
      }
      delay(joystick_delay);
    }

    if (dy > 1000) {
      if (location_index == 0) {
        year--;
      } else if (location_index == 1) {
        if (month > 1) {
          month--;
        }
      } else if (location_index == 2) {
        if (day > 1) {
          day--;
        }
      } else if (location_index == 3) {
        if (hh > 0) {
          hh--;
        }
      } else if (location_index == 4) {
        if (mm > 0) {
          mm--;
        }
      }
      delay(joystick_delay);
    }

    if (dx < 10) {
      if (location_index == 0) {
        location_index = 0;
      } else {
        location_index--;
      }
      delay(joystick_delay);
    }

    if (dx > 1000) {
      if (location_index == 4) {
        location_index = 4;
      } else {
        location_index++;
      }
      delay(joystick_delay);
    }
  }
}

//function
void _init() {
  Serial.begin(115200);
  pinMode(8, INPUT_PULLUP);  // 搖桿按鈕，需上拉電阻
}

void lcd_init() {
  lcd.init();  // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();  //開啟背光
  lcd_CreatChar();
  lcd.setCursor(0, 0);  //col0 row0
  lcd.print("Hello, world!");
  lcd.setCursor(0, 1);  //col0 row1
  lcd.print("Ywrobot Arduino!");
  delay(1000);
  lcd.clear();
}

void lcd_CreatChar() {
  uint8_t yearChar[8] = { 0x10, 0x1f, 0x02, 0x0f, 0x0a, 0x1f, 0x02, 0x00 };   //年字型定義
  uint8_t monthChar[8] = { 0x0f, 0x0a, 0x0f, 0x0a, 0x0f, 0x0a, 0x13, 0x00 };  //月字型定義
  uint8_t dayChar[8] = { 0x0f, 0x09, 0x09, 0x0f, 0x09, 0x09, 0x0f, 0x00 };    //日字型定義

  lcd.createChar(0, yearChar);   //年
  lcd.createChar(1, monthChar);  //月
  lcd.createChar(2, dayChar);    //日
}

void disp_init() {
  //輸出日期的年月日
  lcd.setCursor(4, 0);
  lcd.write(0);
  lcd.setCursor(7, 0);
  lcd.write(1);
  lcd.setCursor(10, 0);
  lcd.write(2);

  //輸出時間的冒號
  lcd.setCursor(2, 1);
  lcd.print(":");
  lcd.setCursor(5, 1);
  lcd.print(":");
}

void Date_Time_Disp() {
  if (edit_mode == false) {
    lcd.noCursor();
    lcd.noBlink();
  } else {
    lcd.cursor();
    lcd.blink();
  }


  //show year
  lcd.setCursor(0, 0);
  lcd.print(year);
  //show month
  lcd.setCursor(5, 0);
  if (month < 10)
    lcd.print("0");
  lcd.print(month);
  //show day
  lcd.setCursor(8, 0);
  if (day < 10)
    lcd.print("0");
  lcd.print(day);

  //show hour
  lcd.setCursor(0, 1);
  if (hh < 10)
    lcd.print("0");
  lcd.print(hh);
  //show min
  lcd.setCursor(3, 1);
  if (mm < 10)
    lcd.print("0");
  lcd.print(mm);

  //show second
  lcd.setCursor(6, 1);
  if (ss < 10)
    lcd.print("0");
  lcd.print(ss);

  //回到編輯模式的游標
  lcd.setCursor(location[location_index] % 16, location[location_index] / 16);

  //時間的冒號閃爍
  if ((millis() - time0) > 500 && edit_mode == false) {
    time0 = millis();
    if (doubleDot == true) {
      lcd.setCursor(2, 1);
      lcd.print(":");
      lcd.setCursor(5, 1);
      lcd.print(":");
      doubleDot = !doubleDot;

      //處理時間進位
      if (++ss == 60) {
        ss = 0;
        if (++mm == 60) {
          mm = 0;
          if (++hh == 24)
            hh = 0;
        }
      }
    } else {
      lcd.setCursor(2, 1);
      lcd.print(" ");
      lcd.setCursor(5, 1);
      lcd.print(" ");
      doubleDot = !doubleDot;
    }
  }
}