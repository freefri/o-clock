// settings.ino — persist menu settings across power loss
// ------------------------------------------------------
// Stores the settings in the AT24C32 EEPROM that sits on the DS3231 module
// (ZS-042). It is a separate chip from the RTC, on the same I2C bus at address
// 0x57, and is truly non-volatile (keeps data with NO power at all — it does
// not even need the coin cell). Works the same on Uno, Nano and ESP32 because
// it is plain I2C.
//
// Saving is debounced: persistSettings() watches the live settings and only writes
// once they have been stable for SETTINGS_SAVE_DELAY ms (i.e. the user stopped
// pressing buttons). That protects the EEPROM and means the menu code does not
// need to call "save" anywhere.

const byte EEPROM_ADDR   = 0x57;   // AT24C32 on the DS3231 / ZS-042 module
const int  SETTINGS_BASE = 0;
const byte SETTINGS_MAGIC = 0xA5;  // marks a valid record (fresh EEPROM reads 0xFF)
const unsigned long SETTINGS_SAVE_DELAY = 2000;  // ms of no change before writing

// EEPROM byte layout — keep it under 32 bytes so it fits in one AT24C32 page.
// [0]=magic  [1]=selectbuzz  [2]=brightness  [3]=secondsToBip  [4]=is24hours
const byte SETTINGS_LEN = 5;

static byte lastSavedBuf[SETTINGS_LEN];  // what is currently in the EEPROM
static byte pendingBuf[SETTINGS_LEN];    // last values we saw (debounce reference)
static bool settingsDirty = false;
static unsigned long settingsSaveAt = 0;

// ---- pack / unpack the live globals <-> a byte buffer ----
static void snapshotSettings(byte* buf) {
  buf[0] = SETTINGS_MAGIC;
  buf[1] = selectbuzz;
  buf[2] = brightness;
  buf[3] = secondsToBip;
  buf[4] = is24hours ? 1 : 0;
}

static void applySettings(const byte* buf) {
  selectbuzz   = buf[1];
  brightness   = buf[2];
  secondsToBip = buf[3];
  is24hours    = buf[4];
}

// ---- raw AT24C32 access (2-byte memory address, big-endian) ----
static void eepromWriteBytes(int memAddr, const byte* data, byte len) {
  Wire.beginTransmission(EEPROM_ADDR);
  Wire.write((byte)(memAddr >> 8));
  Wire.write((byte)(memAddr & 0xFF));
  for (byte i = 0; i < len; i++) Wire.write(data[i]);
  Wire.endTransmission();
  delay(6);  // AT24C32 self-timed write cycle (<= 10 ms); rare, so harmless
}

static void eepromReadBytes(int memAddr, byte* data, byte len) {
  Wire.beginTransmission(EEPROM_ADDR);
  Wire.write((byte)(memAddr >> 8));
  Wire.write((byte)(memAddr & 0xFF));
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)EEPROM_ADDR, (uint8_t)len);
  for (byte i = 0; i < len && Wire.available(); i++) data[i] = Wire.read();
}

// Load saved settings at boot, or seed the EEPROM with defaults on first run.
void loadSettings() {
  byte buf[SETTINGS_LEN];
  eepromReadBytes(SETTINGS_BASE, buf, SETTINGS_LEN);

  if (buf[0] == SETTINGS_MAGIC) {
    applySettings(buf);
  } else {
    snapshotSettings(buf);        // fresh/blank EEPROM -> seed it with the defaults
    eepromWriteBytes(SETTINGS_BASE, buf, SETTINGS_LEN);
  }

  memcpy(lastSavedBuf, buf, SETTINGS_LEN);
  memcpy(pendingBuf, buf, SETTINGS_LEN);
  display.setBrightness(brightness);
}

// Call every loop. Writes to EEPROM only after the settings stop changing.
void persistSettings() {
  byte cur[SETTINGS_LEN];
  snapshotSettings(cur);

  if (memcmp(cur, pendingBuf, SETTINGS_LEN) != 0) {
    // something just changed -> (re)arm the debounce timer, don't write yet
    memcpy(pendingBuf, cur, SETTINGS_LEN);
    settingsSaveAt = millis() + SETTINGS_SAVE_DELAY;
    settingsDirty = true;
    return;
  }

  // settings have been stable; write once the debounce window elapses
  if (settingsDirty && (long)(millis() - settingsSaveAt) >= 0) {
    settingsDirty = false;
    if (memcmp(cur, lastSavedBuf, SETTINGS_LEN) != 0) {
      eepromWriteBytes(SETTINGS_BASE, cur, SETTINGS_LEN);
      memcpy(lastSavedBuf, cur, SETTINGS_LEN);
    }
  }
}
