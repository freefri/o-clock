#include <Adafruit_NeoPixel.h>
#include <DS3231.h>
#include <Wire.h>
#include "glyphs.h"

//******* Messages **********//
const char MSG_SPLASH[]  = "RAZA";
const char MSG_VERSION[] = "V 0.1";

//*******Objects of libraries**********//
DS3231 rtc;

//************ Pin map (per board) ************//
#if defined(ESP32)
  // ESP32-WROOM-32 (38-pin DevKit). Avoid flash pins 6-11 and input-only 34-39.
  const byte PIN_MATRIX    = 19; // Matrix data recommends a 3.3V->5V level shifter
  const byte PIN_BTN_SET   = 27;
  const byte PIN_BTN_PLUS  = 26;
  const byte PIN_BTN_MINUS = 25;
  const byte PIN_BUZZER    = 13;
  const byte PIN_SDA       = 23;
  const byte PIN_SCL       = 22;
  const byte PIN_BTN_GND   = 33; // driven LOW as a ground rail for the buttons
#else
  // Arduino Uno. DS3231 I2C is fixed to A4 (SDA) / A5 (SCL), so no PIN_SDA/PIN_SCL.
  const byte PIN_MATRIX    = 6;
  const byte PIN_BTN_SET   = 4;
  const byte PIN_BTN_PLUS  = 2;
  const byte PIN_BTN_MINUS = 3;
  const byte PIN_BUZZER    = 12;
#endif

Adafruit_NeoPixel display = Adafruit_NeoPixel(256, PIN_MATRIX, NEO_GRB + NEO_KHZ800);

//**********C0NSTANTS***************//

const uint32_t Color_white = display.Color(255, 255, 255);
const uint32_t Color_orange = display.Color(255, 60, 0);
const uint32_t Color_red = display.Color(255, 0, 0);
const uint32_t Color_green = display.Color(10, 255, 10);
const uint32_t Color_blue = display.Color(0, 0, 255);

//*********VARIABLES******************//
byte hh, mm, ss;
byte brightness = 10;
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

char messageBuf[12];
const char* messageText = "";    // brief text shown instead of the clock
uint32_t messageColor = Color_blue;
unsigned long messageUntil = 0;  // ...until this millis() time

/*******define matrix dimensions *****/
#define WIDTH 32
#define HEIGHT 8

//**********SETUP PROGRAM***************//
void setup() {
  Serial.begin(9600);  // only needed for development
  Serial.println("hello -> setup");
  // Buttons: INPUT_PULLUP works on both AVR and ESP32 (the old INPUT + HIGH
  // trick does not enable the pull-up on ESP32).
  pinMode(PIN_BTN_SET, INPUT_PULLUP);
  pinMode(PIN_BTN_PLUS, INPUT_PULLUP);
  pinMode(PIN_BTN_MINUS, INPUT_PULLUP);
#if defined(ESP32)
  pinMode(PIN_BTN_GND, OUTPUT);
  digitalWrite(PIN_BTN_GND, LOW);
#endif
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  // Start the I2C interface
#if defined(ESP32)
  Wire.begin(PIN_SDA, PIN_SCL);   // ESP32 can route I2C to any GPIO
#else
  Wire.begin();                   // AVR: I2C fixed to A4 (SDA) / A5 (SCL)
#endif

  display.begin();
  display.show();
  display.setBrightness(brightness);

  Wire.beginTransmission(0x68);
  bool rtcOk = (Wire.endTransmission() == 0);
  Serial.print("RTC @0x68: ");
  Serial.println(rtcOk ? "FOUND" : "NOT FOUND - check SDA/SCL/3V3/GND wiring");

  if (!rtcOk) {
    drawMessage("NO RTC", Color_red);
    errorBips();
    while (true) delay(1000);  // halt here: keep NO RTC on screen, never start the clock
  }

  rtc.setClockMode(false);

  loadSettings();
  updateClock();
  countDownToBip = 60 - ss;  // align the long bip to second 0 of the RTC

  messageText = MSG_SPLASH;
  messageColor = Color_blue;
  messageUntil = millis() + 1000;
  bootBip();
}

bool colonOn = true;

//**********LOOP PROGRAM***************//
void loop() {
  handleModeButton();
  handleSelectPlusButton();
  handleSelectMinusButton();

  updateClock();
  persistSettings();
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
    if ((long)(millis() - messageUntil) < 0) {
      drawMessage(messageText, messageColor);
    } else {
      colonOn = !colonOn;
      drawClock(hh, mm, ss);
    }
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
  int b0 = digitalRead(PIN_BTN_SET), b1 = digitalRead(PIN_BTN_PLUS), b2 = digitalRead(PIN_BTN_MINUS);
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
