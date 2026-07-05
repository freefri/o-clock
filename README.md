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
- Start intervals of 15s, 30s, 60s or 120s (INT menu), aligned to second :00
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

## Power & boot (ESP32 standalone)
Powering the ESP32 from a power bank via the **5V pin**, with the matrix on its own
feed and all grounds common:

- **Matrix power goes straight from the power bank** (thick wires) — its current is
  too high for the ESP32's traces/USB. The board and matrix only **share ground**.
- **Gotcha: the ESP32 won't boot from the 5V pin as shipped.** With no USB attached,
  the board's auto-reset circuit drags **EN** and **GPIO0** down to a marginal ~2.6 V
  and the chip stays halted — power LEDs on, but no boot bip and a dark matrix. It
  works over USB but looks dead on the power bank.
- **Fix (soldered permanently):** add **two separate pull-up resistors** —
  **1 kΩ: EN → 3V3** and **1 kΩ: GPIO0 (`P0`) → 3V3**. Both pins then sit solidly
  high (~3.2 V) and it boots on every power-up.
  - **Use 1 kΩ, not 4.7 kΩ.** 4.7 kΩ only lifts the pins to ~2.9 V — right on the boot
    threshold, so it becomes a coin flip that depends on resistor tolerance and the
    power bank's ramp. 1 kΩ clears the threshold with real margin.
  - **Keep them separate — do NOT tie EN and P0 to one resistor.** Tying them blocks
    USB flashing: the flasher must hold GPIO0 low *while* pulsing EN to enter download
    mode, which is impossible if the two pins move together. Separate 1 kΩ resistors
    are weak enough that the flasher still wins during upload, yet strong enough to
    win on a bare power-bank boot.
  - **Never use a bare wire** (EN/P0 straight to 3V3, no resistor): pressing EN/BOOT
    then shorts 3V3 to GND, and flashing is blocked. A resistor limits that to ~3 mA.
  - (A `1 µF` cap EN→GND is optional insurance for reset *timing*; not needed once the
    1 kΩ pull-ups make it boot reliably.)

### Flashing after it's soldered
1. **Unplug the power bank**, then plug in the laptop USB — keep only one 5V source live.
2. `./buildnFlash.sh`
3. It reboots after flashing. Running from USB alone, the matrix can exceed a USB
   port's current at high brightness, so either **hold SET while it powers up**
   (clamps brightness to minimum for this session) or reconnect the power bank so the
   matrix has its own supply.

USB VBUS and the 5V pin are diode-isolated on the board (that isolation is exactly
what caused the boot gotcha), so the power bank can't back-feed the laptop even with
both connected — but one-source-at-a-time is the clean habit.
