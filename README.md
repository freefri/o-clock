# o-clock
Bipping clock DIY

The main goal is to be used as Orienteering Starts Clock

## Summary
A DIY clock built to be used as an Orienteering Starts Clock.

### Hardware
- **Microcontroller** — Arduino UNO, Arduino Nano (ATmega328), or ESP32 (the code auto-adapts the pin map per board)
- **Display** — 8×32 WS2812B LED matrix (256 addressable RGB LEDs)
- **Timekeeping RTC** — DS3231 real-time clock module with AT24C32 EEPROM and with a coin-cell backup (LIR2032), keeps time while powered off
- **Controls** — 3 push buttons (SET / + / −) to navigate menus and set the time
- **Sound** — passive buzzer for the interval beeps (two frequencies for warning vs. start)
- **Power** — 5V supply for the matrix and board: a USB outlet, wall adapter, or generic power bank

## Features
- Time can be set manually
- Adjustable brightness
- Start intervals of 30s, 60s or 120s
- Time save, although it is not powered (real time clock with separate battery)
- Different colors for last and start seconds.
- 2 different sounds (two different frequencies from the same buzzer)
- Settings persist across power loss — brightness, buzzer mode and interval are saved to the AT24C32 EEPROM on the DS3231 module and restored on power-up

## Build & flash (command line)
Note: Arduino IDE is recommended to build and flash and install libraries and boards.

Use the helper script `buildnFlash.sh`. It finds `arduino-cli` (bundled inside the
Arduino IDE AppImage, auto-mounting it if needed) and builds + uploads the sketch.
It defaults to ESP32 on the first `/dev/ttyUSB*`/`/dev/ttyACM*` port; pass a FQBN
and port to target another board.

```
./buildnFlash.sh                                    # ESP32 (default)
./buildnFlash.sh arduino:avr:uno /dev/ttyACM0       # Arduino UNO
./buildnFlash.sh esp32:esp32:esp32c3 /dev/ttyACM0   # ESP32-C3 0.42" OLED
```

- If upload fails with **"port ... busy"**, close the Serial Monitor / IDE (it holds the port).
- The ESP32 port sometimes re-enumerates (`/dev/ttyUSB0` ↔ `/dev/ttyUSB1`) after a
  replug — pass the correct one as the 2nd argument.
