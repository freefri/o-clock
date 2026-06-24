#include <Adafruit_NeoPixel.h>
#include <DS3231.h>
#include <Wire.h>
#include "glyphs.h"

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

const uint32_t Color_hhmm = display.Color(255 , 255, 255);  // hours and minutes color - white
const uint32_t Color_orange = display.Color(255, 60, 0) ;    // seconds color - Orange
const uint32_t Color_red = display.Color(255, 0, 0);  // last seconds color - Red
const uint32_t Color_green = display.Color(10, 255, 10); // Seconds 00 color - Green
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

//**** variables to replace rtc ds3231 with milis() *****/
bool rtcAvailable = true;   // set false if RTC not used

byte clock_hh = 12;
byte clock_mm = 0;
byte clock_ss = 0;

unsigned long lastMillis = 0;

/*******define matrix dimensions *****/
#define WIDTH 32
#define HEIGHT 8

//**********SETUP PROGRAM***************//
void setup() {
  Serial.begin(9600); // only needed for development
  Serial.println("hello -> starting");
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

  byte DS3231Address = 0x68;
  Wire.beginTransmission(DS3231Address);
  byte error = Wire.endTransmission();

  if (error != 0) {
    rtcAvailable = false;
  }
}

/*** color blinking */
unsigned long lastBlink = 0;
bool colonOn = true;

const int MODE_BRI = 1;
const int MODE_CLOCK = 0;
const int MODE_BIP = 2;
const int MODE_EDIT = 3;
const int MODE_LAST = 3;

//**********LOOP PROGRAM***************//
void loop() {
  handleModeButton();
  handleSelectPlusButton();

  switch (mode) {
    case MODE_BRI:
      modeBrighness();
      break;

    case MODE_CLOCK:
      modeClock();
      break;

    case MODE_BIP:
      modeBip();
      break;

    case MODE_EDIT:
      modeEdit();
      break;
  }
}

void handleSelectPlusButton() {

  static int lastState = HIGH;
  int state = digitalRead(Pinc);

  if (state == LOW && lastState == HIGH) {

    if (mode == MODE_BIP) {
      selectbuzz++;
      if (selectbuzz > 2) selectbuzz = 0;
    } else if (mode == MODE_BRI) {
      brightness += 10;
      if (brightness > 50) brightness = 10;
      display.setBrightness(brightness);
    }
  }

  lastState = state;
}
void handleModeButton() {

  static int lastState = HIGH;
  int state = digitalRead(Pset);

  if (state == LOW && lastState == HIGH) {
    mode++;
    if (mode > MODE_LAST) mode = 0;
  }

  lastState = state;
}
void modeClock() {
  updateClock();
  colonOn = !colonOn;
  drawClock(hh, mm, ss);
  delay(200);
}
void modeBrighness() {
  display.clear();

  int x = DisplayText("BRI:", 0, 0, Color_hhmm);
  x += Digit2Display(brightness / 10, x, 0, Color_orange);

  display.show();
  delay(200);
}
void modeBip() {
  display.clear();

  int x = DisplayText("BIP:", 0, 0, Color_hhmm);
  if (selectbuzz == 0) {
    x += DisplayText("NO", x, 0, Color_red);
  } else {
    x += Digit2Display(selectbuzz, x, 0, Color_green);
  }

  display.show();
  delay(200);
}
void modeEdit() {
  display.clear();

  int x = DisplayText("EDIT", 0, 0, Color_hhmm);
  if (selectbuzz == 0) {
    x += DisplayText("NO", x, 0, Color_red);
  } else {
    x += Digit2Display(selectbuzz, x, 0, Color_green);
  }

  display.show();
  delay(200);
}
