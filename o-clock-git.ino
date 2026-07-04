#include <Adafruit_NeoPixel.h>
#include <DS3231.h>
#include <Wire.h>
#include "glyphs.h"

//*******Objects of libraries**********//
DS3231 rtc;  //defines the DS3231 as "rtc"

//************ Pin map (per board) ************//
#if defined(ESP32)
  // ESP32-WROOM-32 (38-pin DevKit). Avoid flash pins 6-11 and input-only 34-39.
  const byte ledPin = 19;  // WS2812 matrix data (recommended to add a 3.3V->5V level shifter)
  const byte Pset  = 27;   // mode/SET button
  const byte Pinc  = 26;   // + button
  const byte Pdec  = 25;   // - button
  const byte Buzz  = 13;   // buzzer
  const byte SDA_pin = 23; // DS3231 SDA
  const byte SCL_pin = 22; // DS3231 SCL
  const byte Pgnd  = 33;   // driven LOW as a GND rail for the buttons
#else
  // Arduino Uno
  const byte ledPin = 6;   // WS2812 matrix data (add a 3.3V->5V
  const byte Pset  = 4;    // mode button
  const byte Pinc  = 2;    // + button
  const byte Pdec  = 3;    // - button
  const byte Buzz  = 12;   // buzzer
  // DS3231 I2C is fixed to A4 (SDA) / A5 (SCL)
#endif

Adafruit_NeoPixel display = Adafruit_NeoPixel(256, ledPin, NEO_GRB + NEO_KHZ800);  //sets all the parameters of the display

//**********C0NSTANTS***************//

const uint32_t Color_white = display.Color(255, 255, 255);  // hours and minutes color - white
const uint32_t Color_orange = display.Color(255, 60, 0);    // seconds color - Orange
const uint32_t Color_red = display.Color(255, 0, 0);        // last seconds color - Red
const uint32_t Color_green = display.Color(10, 255, 10);    // Seconds 00 color - Green
const uint32_t Color_blue = display.Color(0, 0, 255); // blue

//*********VARIABLES******************//
byte hh, mm, ss;                   // hours, minutes, seconds
byte brightness = 10;              //Maximum brightness of 255
const int MAX_BRIGHTNESS = 20;
bool is24hours = false;

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
bool bipToneOn = false;                 // true while the buzzer is sounding (see bip.ino)

byte lastSs = 255;  // last seconds seen by the bip countdown

/*******define matrix dimensions *****/
#define WIDTH 32
#define HEIGHT 8

//**********SETUP PROGRAM***************//
void setup() {
  Serial.begin(9600);  // only needed for development
  Serial.println("hello -> setup");
  // Buttons: INPUT_PULLUP works on both AVR and ESP32 (the old INPUT + HIGH
  // trick does not enable the pull-up on ESP32).
  pinMode(Pset, INPUT_PULLUP);
  pinMode(Pinc, INPUT_PULLUP);
  pinMode(Pdec, INPUT_PULLUP);
#if defined(ESP32)
  pinMode(Pgnd, OUTPUT);
  digitalWrite(Pgnd, LOW);   // P33 acts as a GND rail for the buttons (tiny current only)
#endif
  pinMode(Buzz, OUTPUT);
  digitalWrite(Buzz, LOW);

  // Start the I2C interface
#if defined(ESP32)
  Wire.begin(SDA_pin, SCL_pin);   // ESP32 can route I2C to any GPIO
#else
  Wire.begin();                   // AVR: I2C fixed to A4 (SDA) / A5 (SCL)
#endif

  // bring-up debug: is the DS3231 answering on the I2C bus?
  Wire.beginTransmission(0x68);
  Serial.print("RTC @0x68: ");
  Serial.println(Wire.endTransmission() == 0 ? "FOUND" : "NOT FOUND - check SDA/SCL/3V3/GND wiring");

  display.begin();                    //Starts the Neopixel display
  display.show();                     //Initialize all pixels to 'off'
  display.setBrightness(brightness);  //sets brightness using variable set earlier

  rtc.setClockMode(false);  // 24h mode

  updateClock();
  countDownToBip = 60 - ss;  // align the long bip to second 0 of the RTC
}

/*** color blinking */
bool colonOn = true;

//**********LOOP PROGRAM***************//
void loop() {
  handleModeButton();
  handleSelectPlusButton();
  handleSelectMinusButton();

  updateClock();
  debugSerial();  // bring-up: RTC time + button states over USB (remove later)

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
  if (!bipToneOn) {
    colonOn = !colonOn;
    drawClock(hh, mm, ss);
  }
  bip();
  delay(200);
}
int prevBip = 0;
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

// Bring-up debug: once per second, print the RTC time and the 3 button states.
// Buttons read 1 when released, 0 when pressed (hold one to test the wiring).
void debugSerial() {
  static byte lastSs = 255;
  static int lastBtns = -1;
  int b0 = digitalRead(Pset), b1 = digitalRead(Pinc), b2 = digitalRead(Pdec);
  int btns = (b0 << 2) | (b1 << 1) | b2;
  // Print on each second tick OR whenever a button changes (catches every press).
  if (ss == lastSs && btns == lastBtns) return;
  lastSs = ss;
  lastBtns = btns;

  Serial.print("t=");
  if (hh < 10) Serial.print('0');
  Serial.print(hh);
  Serial.print(':');
  if (mm < 10) Serial.print('0');
  Serial.print(mm);
  Serial.print(':');
  if (ss < 10) Serial.print('0');
  Serial.print(ss);

  Serial.print("  btn set/+/- = ");
  Serial.print(b0);
  Serial.print('/');
  Serial.print(b1);
  Serial.print('/');
  Serial.println(b2);
}
