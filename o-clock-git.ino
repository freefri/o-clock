#include <Adafruit_NeoPixel.h>
#include <DS3231.h>
#include <Wire.h>
#include "glyphs.h"

//*******Objects of libraries**********//
DS3231 rtc;  //defines the DS3231 as "rtc"

const byte ledPin = 6;                                                             // display control pin
Adafruit_NeoPixel display = Adafruit_NeoPixel(256, ledPin, NEO_GRB + NEO_KHZ800);  //sets all the parameters of the display

//************Buttons****************//
const byte Pset = 4;
const byte Pdec = 3;
const byte Pinc = 2;

//************I/O********************//
const byte Buzz = 12; //12 new design, 13 old
const byte Buzz2 = 8; // loud buzz

//**********C0NSTANTS***************//

const uint32_t Color_white = display.Color(255, 255, 255);  // hours and minutes color - white
const uint32_t Color_orange = display.Color(255, 60, 0);    // seconds color - Orange
const uint32_t Color_red = display.Color(255, 0, 0);        // last seconds color - Red
const uint32_t Color_green = display.Color(10, 255, 10);    // Seconds 00 color - Green
const uint32_t Color_blue = display.Color(0, 0, 255); // blue

//*********VARIABLES******************//
byte YY, MM, DD, dOW, hh, mm, ss;  // year, month, day, DayOfWeek, hours, minutes, seconds
byte brightness = 10;              //Maximum brightness of 255
byte aux1, aux2;
byte cs;  // color of seconds: 0=normal 1=last_seconds 2=start_second

// Mode menu
byte mode = 0;
const byte MODE_BRIGHTNESS = 1;
const byte MODE_CLOCK = 0;
const byte MODE_BIP = 2;
const byte MODE_EDIT = 3;
const byte MODE_LAST = 3;
byte submode_editing = 0;
const byte EDIT_MODE_HH = 1;
const byte EDIT_MODE_MM = 2;
const byte EDIT_MODE_SS = 3;
// END Mode menu

byte secondsToBip = 15; // period of buzzer in seconds
byte selectbuzz = 1; // 0= no buzzer, 1= normal, 2=loud sound
byte countDownToBip = secondsToBip - 1; // seconds remaining to start

//**** variables to replace rtc ds3231 with milis() *****/
bool rtcAvailable = true;  // set false if RTC not used

byte clock_hh = 12;
byte clock_mm = 0;
byte clock_ss = 50;

unsigned long lastMillis = 0;
byte lastSs = 255;

/*******define matrix dimensions *****/
#define WIDTH 32
#define HEIGHT 8

//**********SETUP PROGRAM***************//
void setup() {
  Serial.begin(9600);  // only needed for development
  Serial.println("hello -> setup");
  // Buttons input setting
  pinMode(Pset, INPUT);
  digitalWrite(Pset, HIGH);
  pinMode(Pinc, INPUT);
  digitalWrite(Pinc, HIGH);
  pinMode(Pdec, INPUT);
  digitalWrite(Pdec, HIGH);
  pinMode(Buzz, OUTPUT);
  digitalWrite(Buzz, LOW);
  pinMode(Buzz2, OUTPUT);
  digitalWrite(Buzz, LOW);

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
  updateClock();
  countDownToBip = 60 - clock_ss;
}

/*** color blinking */
unsigned long lastBlink = 0;
bool colonOn = true;

//**********LOOP PROGRAM***************//
void loop() {
  handleModeButton();
  handleSelectPlusButton();
  handleSelectMinusButton();

  updateClock();

  switch (mode) {
    case MODE_BRIGHTNESS:
      modeBrightness();
      break;

    case MODE_CLOCK:
      modeClock();
      break;

    case MODE_BIP:
      modeBip();
      break;

    case MODE_EDIT:
      if (submode_editing == 0) {
        modeEdit();
      } else {
        modeClock();
      }
      break;
  }
}

void modeClock() {
  colonOn = !colonOn;
  drawClock(hh, mm, ss);
  bip();
  delay(200);
}
int prevBip = 0;
void bip() {
  if (prevBip == countDownToBip) {
    return;
  }
  prevBip = countDownToBip;
  if (selectbuzz == 0) {
    return;
  }
  int tone1 = 400;
  tone1 = tone1 * selectbuzz;
  int tone2 = 125;
  if (countDownToBip == 1) {
    tone(Buzz, tone1);
  } else if (countDownToBip == 10) {
    tone(Buzz, tone1);
  } else if (countDownToBip == 0) {
    tone(Buzz, tone1 + tone2);
    delay(400);
  } else if (countDownToBip <= 5 && countDownToBip > 1) {
    tone(Buzz, tone1);
  }
  delay(200);
  noTone(Buzz);
}
void modeBrightness() {
  display.clear();

  int x = DisplayText("BRI:", 0, 0, Color_white);
  x += Digit2Display(brightness / 10, x, 0, Color_orange);

  display.show();
  delay(200);
}
void modeBip() {
  display.clear();

  int x = DisplayText("BIP:", 0, 0, Color_white);
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

  int x = DisplayText("EDIT", 0, 0, Color_orange);
  display.show();
  delay(200);
}
