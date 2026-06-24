#include <Adafruit_NeoPixel.h>
#include <DS3231.h>
#include <Wire.h>

//*******Objects of libraries**********//
DS3231 rtc;  //defines the DS3231 as "rtc"

const byte ledPin = 6;                                                             // display control pin
Adafruit_NeoPixel display = Adafruit_NeoPixel(256, ledPin, NEO_GRB + NEO_KHZ800);  //sets all the parameters of the display

//************Buttons****************//
const byte Pset = 4;  //4 new design, 9 old
const byte Pinc = 3; //3 new design, 10 old
const byte Pdec = 2; //2 new design, 11 old

//************I/O********************//
const byte Buzz = 12; //12 new design, 13 old
const byte Buzz2 = 8; // loud buzz

//**********C0NSTANTS***************//
bool DIGIT[20][32] = { { 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0 },    //digit 0
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },    //digit 1
  { 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0 },    //digit 2
  { 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0 },    //digit 3
  { 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0 },    //digit 4
  { 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0 },    //digit 5
  { 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0 },    //digit 6
  { 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0 },    //digit 7
  { 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0 },    //digit 8
  { 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0 },    //digit 9
  { 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1 },    //digit _0
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1 },    //digit _1
  { 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1 },    //digit _2
  { 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1 },    //digit _3
  { 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1 },    //digit _4
  { 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1 },    //digit _5
  { 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1 },    //digit _6
  { 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1 },    //digit _7
  { 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1 },    //digit _8
  { 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1 }      //digit _9
};

const uint32_t Color_hhmm = display.Color(255 , 255, 255);  // hours and minutes color - white
const uint32_t Color_ss = display.Color(255, 60, 0) ;    // seconds color - Orange
const uint32_t Color_ss55 = display.Color(255, 0, 0);  // last seconds color - Red
const uint32_t Color_ss00 = display.Color(10, 255, 10); // Seconds 00 color - Green
const unsigned int shortbuzz = 200u; // short sound duration
const unsigned int longbuzz = 800u; // long sound duration

//*********VARIABLES******************//
bool h12;                      // 12h format
bool pm;                       // flag pm
byte YY, MM, DD, dOW, hh, mm, ss;  // year, month, day, DayOfWeek, hours, minutes, seconds
byte  hhd, mmd, ssd, moded;       //  hours, minutes, seconds, mode shown currently in display
byte brightness = 10;             //Maximum brightness of 255
byte aux1, aux2;
byte cs;                          // color of seconds: 0=normal 1=last_seconds 2=start_second

byte mode = 0;  // Mode status: 0=normal with brithness adjust 1=setting hour 2=setting minute 3=setting second 4=set buzzer period 5=select buzzer
byte perbuzz = 60; // period of buzzer in seconds
byte selectbuzz = 1; // 0= no buzzer, 1= normal, 2=loud sound
byte secbuzz; // seconds remaining to start
unsigned long  millsinibuzz = millis(); // ms of start of buzz
unsigned int ssinibuzz = 1; //second in which buzz has started
unsigned int buzzeroff; //lenght of buzz
bool buzzstate = 0;
int butreading; // read of current status of a button
int buttonState [3] ; // current reading status of buttons menu, + and -
int lastButtonState [3] = {LOW, LOW, LOW};
unsigned long lastDebounceTime[3] = {0L, 0L, 0L};
unsigned long debounceDelay = 5L;  // ms of debounce


//**********SETUP PROGRAM***************//
void setup() {
  // Buttons input setting
  pinMode (Pset, INPUT); digitalWrite(Pset, HIGH);
  pinMode (Pinc, INPUT); digitalWrite(Pinc, HIGH);
  pinMode (Pdec, INPUT); digitalWrite(Pdec, HIGH);
  pinMode (Buzz, OUTPUT); digitalWrite(Buzz, LOW);
  pinMode (Buzz2, OUTPUT); digitalWrite(Buzz, LOW);

  // Start the I2C interface
  Wire.begin();

  display.begin();                    //Starts the Neopixel display
  display.show();                     //Initialize all pixels to 'off'
  display.setBrightness(brightness);  //sets brightness using variable set earlier

  rtc.setClockMode(false);
}

//**********LOOP PROGRAM***************//
void loop() {

  hh = rtc.getHour(h12, pm);
  mm = rtc.getMinute();
  ss = rtc.getSecond();

  // update display
  if ((mode == 4) or (mode == 5)) { //  4 = mode set buzz period, 5 = select buzz
    aux1 = perbuzz >= 100 ? perbuzz / 100 : 255;
    Chars2Display(selectbuzz, 255,  255,  aux1 , (perbuzz % 100) / 10, perbuzz % 10, mode, 0);
  }
  if (mode >= 0 && mode <= 3) { // modes 0, 1, 2, 3 (normal or changing hour min or second)
    if ((hh != hhd) || (mm != mmd) || (ss != ssd) || mode != moded) {
      if (perbuzz != 0) {
        secbuzz = (perbuzz - (((mm % 2) * 60 + ss ) % perbuzz)) % perbuzz; //seconds to the end of buzzer period (p-1, p-2 ... 2, 1, 0, p-1...)
      }
      if (secbuzz == 0) {
        Chars2Display(hh / 10, hh % 10, mm / 10, mm % 10, ss / 10, ss % 10, mode, 2); // print time of start second
      } else if (secbuzz < 6) {
        Chars2Display(hh / 10, hh % 10, mm / 10, mm % 10, ss / 10, ss % 10, mode, 1); // print time of last seconds to start
      } else {
        Chars2Display(hh / 10, hh % 10, mm / 10, mm % 10, ss / 10, ss % 10, mode, 0); // print normal time
      }

      hhd = hh;
      mmd = mm;
      ssd = ss;
      moded = mode;
    }
  }


  // ********************buzzer actions******************************

  if (secbuzz < 6  && mode < 4) {
    buzzeroff = (secbuzz == 0) ? longbuzz : shortbuzz;  // in second 0, long buzz; for the rest, short buzz
    if ( buzzstate == 0 && ssinibuzz != ss ) { //starts buzz
      buzzstate = 1;
      millsinibuzz = millis();
      ssinibuzz = ss;
      if (selectbuzz == 1) {
        digitalWrite(Buzz, HIGH);
      } else if (selectbuzz == 2) {
        digitalWrite(Buzz2 , HIGH);
      }

    } else if (millis() >= (millsinibuzz + long(buzzeroff)) && buzzstate == 1) { //end of buzz
      //Serial.print("butt= ");    Serial.println(millis()-(millsinibuzz + long(buzzeroff)));
      buzzstate = 0;
      digitalWrite(Buzz, LOW);
      digitalWrite(Buzz2, LOW);
      //buzzeroff = 10000;
    }
  } else if (secbuzz >= 5 ||  mode == 4 ) {
    digitalWrite (Buzz, LOW);
  }


  // ********************Buttons overview******************************
  //  button "menu"
  butreading = digitalRead(Pset);
  //Serial.print("butt= ");    Serial.println(butreading);
  if (butreading != lastButtonState[0]) { //si cambia estado reinicia temporizador de pulsación
    lastDebounceTime[0] = millis();
  }
  if ((millis() - lastDebounceTime[0]) > debounceDelay) {
    if (butreading != buttonState[0]) {
      buttonState[0] = butreading;
      if (buttonState[0] == LOW) {
        mode = mode + 1;
        if (mode > 5) {
          mode = 0;
        }
      }
    }
  }
  lastButtonState[0] = butreading;

  //  button "+"
  butreading = digitalRead(Pinc);
  //Serial.print("butt= ");    Serial.println(butreading);
  if (butreading != lastButtonState[1]) { //si cambia estado reinicia temporizador de pulsación
    lastDebounceTime[1] = millis();
  }
  if ((millis() - lastDebounceTime[1]) > debounceDelay) {
    if (butreading != buttonState[1]) {
      buttonState[1] = butreading;
      if (buttonState[1] == LOW) {
        switch (mode) {
          case 0:
            if (brightness < 100) {
              brightness = brightness + 10;
              display.show();
              display.setBrightness(brightness);
            }
            aux1 = brightness / 100;
            aux2 = (brightness / 10) % 10;
            Chars2Display(255, 255, aux1, aux2, 255, 255,  mode, 0);
            delay(10);
            break;
          case 1:
            if (hh == 23) {
              rtc.setHour(0);
            }   else {
              rtc.setHour(hh + 1);
            }
            break;
          case 2:
            if (mm == 59) {
              rtc.setMinute(0);
            }   else {
              rtc.setMinute(mm + 1);
            }
            break;
          case 3:
            if (ss == 59) {
              rtc.setSecond(0);
            }   else {
              rtc.setSecond(ss + 1);
            }
            break;
          case 4:
            switch (perbuzz) {
              case 0:
                perbuzz = 30;
                break;
              case 30:
                perbuzz = 60;
                break;
              case 60:
                perbuzz = 120;
                break;
              case 120:
                perbuzz = 0;
                secbuzz = 255;
                break;
            }
            break;
          case 5:
            switch (selectbuzz) {
              case 0:
                selectbuzz = 1;
                digitalWrite(Buzz, HIGH);
                delay (100);
                digitalWrite(Buzz, LOW);
                break;
              case 1:
                selectbuzz = 2;
                digitalWrite(Buzz2, HIGH);
                delay(100);
                digitalWrite(Buzz2, LOW);
                break;
              case 2:
                selectbuzz = 0;
                break;
            }
            break;
        }
      }
    }
  }
  lastButtonState[1] = butreading;

  //  button "-"
  butreading = digitalRead(Pdec);
  //Serial.print("butt= ");    Serial.println(butreading);
  if (butreading != lastButtonState[2]) { //si cambia estado reinicia temporizador de pulsación
    lastDebounceTime[2] = millis();
  }
  if ((millis() - lastDebounceTime[2]) > debounceDelay) {
    if (butreading != buttonState[2]) {
      buttonState[2] = butreading;
      if (buttonState[2] == LOW) {
        switch (mode) {
          case 0:
            if (brightness > 10) {
              brightness = brightness - 10;
              display.show();
              display.setBrightness(brightness);
            }
            aux1 = brightness / 100;
            aux2 = (brightness / 10) % 10;
            Chars2Display(255, 255, aux1, aux2, 255, 255,  mode, 0);
            delay(10);
            break;
          case 1:
            if (hh == 00) {
              rtc.setHour(23);
            }   else {
              rtc.setHour(hh - 1);
            }
            break;
          case 2:
            if (mm == 00) {
              rtc.setMinute(59);
            }   else {
              rtc.setMinute(mm - 1);
            }
            break;
          case 3:
            if (ss == 00) {
              rtc.setSecond(59);
            }   else {
              rtc.setSecond(ss - 1);
            }
            break;
          case 4:
            switch (perbuzz) {
              case 0:
                perbuzz = 120;
                break;
              case 30:
                perbuzz = 0;
                secbuzz = 255;
                break;
              case 60:
                perbuzz = 30;
                break;
              case 120:
                perbuzz = 60;
                break;
            }
            break;
          case 5:
            switch (selectbuzz) {
              case 0:
                selectbuzz = 2;
                digitalWrite(Buzz2, HIGH);
                delay(100);
                digitalWrite(Buzz2, LOW);
                break;
              case 1:
                selectbuzz = 0;
                break;
              case 2:
                selectbuzz = 1;
                digitalWrite(Buzz, HIGH);
                delay (100);
                digitalWrite(Buzz, LOW);
                break;
            }
            break;
        }
      }
    }
  }
  lastButtonState[2] = butreading;


  //delay(50);  //Delays 50 milliseconds
}

//****************OTHER SUBRUTINES********************//

void Chars2Display(byte d1, byte d2, byte d3, byte d4, byte d5, byte d6, byte mode, byte cs) {
  // draw the 6 digits in the display.
  // mode is used to choose the mode of represent: 0 = time with seconds colored; 1,2,3 = setting h, m, s (underlined); 4 = buzzer period underlined; 5 = buzzer selected underlined
  // cs is color of seconds: 0=normal 1=last_seconds 2=start_second
  byte digit;
  int cursor;

  for (int h = 1; h <= 6; h++) {
    switch (h) {
      case 1:  //hour decene
        digit = d1;
        cursor = 224;
        if ((mode == 1) or (mode == 5)) {
          digit = digit + 10; //underline in case of changing time mode or buzzer select mode
        }
        Digit2Display(digit, cursor, Color_hhmm);
        break;
      case 2:  //hour unit
        digit = d2;
        cursor = 184;
        if (mode == 1) {
          digit = digit + 10; //underline in case of changing time mode
        }
        Digit2Display(digit, cursor, Color_hhmm);
        break;
      case 3:  //minute decene
        digit = d3;
        cursor = 128;
        if (mode == 2) {
          digit = digit + 10; //underline in case of changing time mode
        }
        Digit2Display(digit, cursor, Color_hhmm);
        break;
      case 4:  //minute unit
        digit = d4;
        cursor = 88;
        if ((mode == 2) or ((mode == 4) and (digit < 255))) {
          digit = digit + 10; //underline in case of changing time mode or buzzer period mode
        }
        Digit2Display(digit, cursor, Color_hhmm);
        break;
      case 5:  //second decene
        digit = d5;
        cursor = 40;
        if ((mode == 3) or (mode == 4)) {
          digit = digit + 10; //underline in case of changing time mode or buzzer period mode
        }
        if (cs == 1 && mode < 4) {
          Digit2Display(digit, cursor, Color_ss55);
        } else if (cs == 2 && mode < 4) {
          Digit2Display(digit, cursor, Color_ss00);
        } else if (mode < 4) {
          Digit2Display(digit, cursor, Color_ss);
        } else {
          Digit2Display(digit, cursor, Color_hhmm);
        }
        break;
      case 6:  //second unit
        digit = d6;
        cursor = 0;
        if ((mode == 3) or (mode == 4)) {
          digit = digit + 10; //underline in case of changing time mode or buzzer period mode
        }
        if (cs == 1  && mode < 4) {
          Digit2Display(digit, cursor, Color_ss55);
        } else if (cs == 2 && mode < 4) {
          Digit2Display(digit, cursor, Color_ss00);
        } else if (mode < 4) {
          Digit2Display(digit, cursor, Color_ss);
        } else {
          Digit2Display(digit, cursor, Color_hhmm);
        }
        break;
    }
    // separators between hour and minutes
    if ((ss % 2 == 0) && (mode < 4)) {
      display.setPixelColor(173, Color_hhmm);
      display.setPixelColor(170, Color_hhmm);
    } else if ((ss % 2 == 1) || (mode > 3)) {
      display.setPixelColor(173, display.Color(0, 0, 0));
      display.setPixelColor(170, display.Color(0, 0, 0));
    }
  }

  display.show();
}


void Digit2Display(byte chr, int cursor, uint32_t color) {

  //Serial.print("debug= ");    Serial.println(chr);
  // Shows a digit ("chr" row of DIGIT) of "color" beggining in the led "cursor" of the display
  int k;

  if (chr == 255) {   // 255: character " "
    for ( k = 1; k <= 32; k++) {
      display.setPixelColor(cursor, display.Color(0, 0, 0));
      cursor++;
    }
  } else {
    if (cursor % 16 == 0) {  // shows a digit in a even column
      for ( k = 1; k <= 32; k++) {
        if (DIGIT[chr][32 - k] == 1) {
          display.setPixelColor(cursor, color);
        } else if (DIGIT[chr][32 - k] == 0) {
          display.setPixelColor(cursor, display.Color(0, 0, 0));
        }
        cursor++;
      }
    } else if (cursor % 16 == 8) {  // shows a digit in a odd column
      //Serial.print("cursor= ");    Serial.println(cursor);
      for (int l = 0; l <= 3; l++) {
        //Serial.print("l= ");    Serial.println(l);
        for (int k = 8; k >= 1; k--) {
          if (DIGIT[chr][32 - 8 * l - k] == 1) {
            display.setPixelColor(cursor, color);
          } else if (DIGIT[chr][32 - 8 * l - k] == 0) {
            display.setPixelColor(cursor, display.Color(0, 0, 0));
          }
          cursor++;
        }
      }
    }
  }
}
