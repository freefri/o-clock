# o-clock
Bipping clock DIY

The main goal is to be used as Orienteering Starts Clock

## Summary
I am doing a DIY project of a clock with the following hardware:
Arduino UNO, 8×32 WS2812B matrix (256 LEDs), 3 push buttons to change settings, a buzzer to emit interval sounds.
RTC DS3231 (LIR2032) to keep time while off, arduino nano 328.

## Features
- Time can be set manually
- Adjustable brightness
- Start intervals of 30s, 60s or 120s
- Time save, although it is not powered (real time clock with separate battery)
- Different colors for last and start seconds.
- 2 different sounds (2 buzzers)

## Build & flash (command line)
Board: Arduino UNO, FQBN `arduino:avr:uno`, port `/dev/ttyACM0`.

```
arduino-cli compile --fqbn arduino:avr:uno o-clock-git
arduino-cli compile --upload -p /dev/ttyACM0 --fqbn arduino:avr:uno o-clock-git
```

- `arduino-cli` ships inside the Arduino IDE AppImage. To use it when the IDE is
  closed, mount the AppImage without launching the GUI:
  `./arduino-ide_*.AppImage --appimage-mount` (prints a `/tmp/.mount_...` path;
  `arduino-cli` is under `resources/app/lib/backend/resources/`).
- If upload fails with **"port /dev/ttyACM0 busy"**, close the Serial Monitor / IDE
  (it holds the port).
