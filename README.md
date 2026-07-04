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

## Build & flash (command line)
Board: Arduino UNO, FQBN `arduino:avr:uno`, port `/dev/ttyACM0`.

```
arduino-cli compile --fqbn arduino:avr:uno o-clock-git
# connect to Arduino UNO
arduino-cli compile --upload -p /dev/ttyACM0 --fqbn arduino:avr:uno o-clock-git
# connect to ESP32 (ESP32 Dev Module)
arduino-cli compile --upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32 o-clock-git
# connect to ESP32-C3 0.42" OLED board (ESP32C3 Dev Module, native USB)
arduino-cli compile --upload -p /dev/ttyACM0 --fqbn esp32:esp32:esp32c3 o-clock-git
```

- `arduino-cli` ships inside the Arduino IDE AppImage. To use it when the IDE is
  closed, mount the AppImage without launching the GUI:
  `./arduino-ide_*.AppImage --appimage-mount` (prints a `/tmp/.mount_...` path;
  `arduino-cli` is under `resources/app/lib/backend/resources/`).
- If upload fails with **"port /dev/ttyACM0 busy"**, close the Serial Monitor / IDE
  (it holds the port).
