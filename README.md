# o-clock
Bipping clock DIY

The main goal is to be used as Orienteering Starts Clock

> ⚠️ **Disconnect the external power supply (power bank) before connecting the USB C to the laptop to debug/flash.**

## Summary
A DIY clock built to be used as an Orienteering Starts Clock.

### Hardware
- **Microcontroller** — Arduino UNO, Arduino Nano (ATmega328), or [ESP32S](https://es.aliexpress.com/item/1005011840612155.html) (the code auto-adapts the pin map per board)
- **Display** — 8×32 [WS2812B LED matrix](https://es.aliexpress.com/item/4001296811800.html) (256 addressable RGB LEDs)
- **[Timekeeping RTC](https://es.aliexpress.com/item/1005007143542894.html)** — DS3231 real-time clock module with AT24C32 EEPROM and with a coin-cell backup (LIR2032), keeps time while powered off
- **Controls** — [3 push buttons](https://es.aliexpress.com/item/33000051703.html) (SET / + / −) to navigate menus and set the time
- **Sound** — [passive buzzer](https://es.aliexpress.com/item/1005010509927983.html) for the countdown / interval beeps
- **Power** — 5V supply for the matrix and board: a USB outlet, wall adapter, or generic power bank

## Features
- Time can be set manually
- Adjustable brightness
- Start intervals of 15s, 30s, 60s or 120s (INT menu), aligned to second :00
- Time save, although it is not powered (real time clock with separate battery)
- Different colors for last and start seconds.
- Selectable buzzer (BIP menu): off, or 3 tone modes (short-high / low / high)
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
```

> ⚠️ **To flash the soldered clock:** Disconnect the external power supply (power bank) before connecting the USB C to the laptop to debug/flash.

- If upload fails with **"port ... busy"**, close the Serial Monitor / IDE (it holds the port).
- The ESP32 port sometimes re-enumerates (`/dev/ttyUSB0` ↔ `/dev/ttyUSB1`) after a
  replug — pass the correct one as the 2nd argument.

## Power & boot (ESP32 standalone)
- Feed the matrix 5V straight from the power bank (thick wires); board and matrix share ground only.
- Feed the ESP32 from the power bank via the **5V pin**.
- Add two separate pull-up resistors: **1 kΩ: EN → 3V3** and **1 kΩ: P0 (GPIO0) → 3V3**. Do not tie EN and P0 together. (Specific to certain DevKit's auto-reset; other ESP32 boards may need only one pull-up, or none. The board-agnostic alternative is to power the ESP32 through the USB connector (VBUS) instead of the 5V pin)
