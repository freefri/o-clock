// Save/restore menu settings across power loss, using the AT24C32 EEPROM
// (I2C 0x57) on the DS3231 module. Writes are debounced to spare the EEPROM.
// Plain I2C, so it works on Uno, Nano and ESP32.

const byte EEPROM_ADDR   = 0x57;
const int  SETTINGS_BASE = 0;
const byte SETTINGS_MAGIC = 0xA5;  // a fresh EEPROM reads 0xFF, so this marks a valid record
const unsigned long SETTINGS_SAVE_DELAY = 2000;  // ms of no change before a write

// Byte layout (keep < 32 bytes to fit one page):
// [0]=magic [1]=selectbuzz [2]=brightness [3]=secondsToBip [4]=is24hours
const byte SETTINGS_LEN = 5;

static byte lastSavedBuf[SETTINGS_LEN];  // mirror of what is in the EEPROM
static byte pendingBuf[SETTINGS_LEN];    // debounce reference
static bool settingsDirty = false;
static unsigned long settingsSaveAt = 0;

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

static void eepromWriteBytes(int memAddr, const byte* data, byte len) {
  Wire.beginTransmission(EEPROM_ADDR);
  Wire.write((byte)(memAddr >> 8));
  Wire.write((byte)(memAddr & 0xFF));
  for (byte i = 0; i < len; i++) Wire.write(data[i]);
  Wire.endTransmission();
  delay(6);  // AT24C32 write cycle (<= 10 ms)
}

static void eepromReadBytes(int memAddr, byte* data, byte len) {
  Wire.beginTransmission(EEPROM_ADDR);
  Wire.write((byte)(memAddr >> 8));
  Wire.write((byte)(memAddr & 0xFF));
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)EEPROM_ADDR, (uint8_t)len);
  for (byte i = 0; i < len && Wire.available(); i++) data[i] = Wire.read();
}

void loadSettings() {
  byte buf[SETTINGS_LEN];
  eepromReadBytes(SETTINGS_BASE, buf, SETTINGS_LEN);

  if (buf[0] == SETTINGS_MAGIC) {
    applySettings(buf);
  } else {
    snapshotSettings(buf);   // first run: seed the EEPROM with current defaults
    eepromWriteBytes(SETTINGS_BASE, buf, SETTINGS_LEN);
  }

  memcpy(lastSavedBuf, buf, SETTINGS_LEN);
  memcpy(pendingBuf, buf, SETTINGS_LEN);
  display.setBrightness(brightness);
}

// Call every loop; writes only after the settings have stopped changing.
void persistSettings() {
  byte cur[SETTINGS_LEN];
  snapshotSettings(cur);

  if (memcmp(cur, pendingBuf, SETTINGS_LEN) != 0) {
    memcpy(pendingBuf, cur, SETTINGS_LEN);   // still changing -> restart the timer
    settingsSaveAt = millis() + SETTINGS_SAVE_DELAY;
    settingsDirty = true;
    return;
  }

  if (settingsDirty && (long)(millis() - settingsSaveAt) >= 0) {
    settingsDirty = false;
    if (memcmp(cur, lastSavedBuf, SETTINGS_LEN) != 0) {
      eepromWriteBytes(SETTINGS_BASE, cur, SETTINGS_LEN);
      memcpy(lastSavedBuf, cur, SETTINGS_LEN);
    }
  }
}
