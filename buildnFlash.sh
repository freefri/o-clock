#!/usr/bin/env bash
# Build and flash the o-clock sketch.
# Usage: ./buildnFlash.sh [fqbn] [port]
#   defaults: fqbn = esp32:esp32:esp32, port = first /dev/ttyUSB* or /dev/ttyACM*
# Examples:
#   ./buildnFlash.sh                                    # ESP32 (default)
#   ./buildnFlash.sh arduino:avr:uno /dev/ttyACM0       # Arduino UNO
#   ./buildnFlash.sh esp32:esp32:esp32c3 /dev/ttyACM0   # ESP32-C3 0.42" OLED

SKETCH_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FQBN="${1:-esp32:esp32:esp32}"
PORT="${2:-$(ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null | head -1)}"

# arduino-cli: prefer one on PATH, else the copy bundled in the Arduino IDE AppImage,
# mounting the AppImage on demand if it is not already mounted.
CLI="$(command -v arduino-cli 2>/dev/null)"
[ -z "$CLI" ] && CLI="$(ls /tmp/.mount_arduin*/resources/app/lib/backend/resources/arduino-cli 2>/dev/null | head -1)"
if [ -z "$CLI" ]; then
  APPIMAGE="$(ls "$HOME"/Arduino/arduino-ide_*.AppImage 2>/dev/null | head -1)"
  if [ -n "$APPIMAGE" ]; then
    echo "Mounting Arduino IDE AppImage for arduino-cli..."
    "$APPIMAGE" --appimage-mount >/dev/null 2>&1 &
    sleep 4
    CLI="$(ls /tmp/.mount_arduin*/resources/app/lib/backend/resources/arduino-cli 2>/dev/null | head -1)"
  fi
fi

[ -z "$CLI" ]  && { echo "ERROR: arduino-cli not found. Install it, or put the Arduino IDE AppImage in ~/Arduino/." >&2; exit 1; }
[ -z "$PORT" ] && { echo "ERROR: no serial port found. Plug in the board or pass one: $0 $FQBN /dev/ttyUSB0" >&2; exit 1; }

echo "CLI:    $CLI"
echo "FQBN:   $FQBN"
echo "PORT:   $PORT"
echo "SKETCH: $SKETCH_DIR"
echo

exec "$CLI" compile --upload -p "$PORT" --fqbn "$FQBN" "$SKETCH_DIR"
