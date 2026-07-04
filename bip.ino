unsigned long bipToneUntil = 0;  // millis() time at which to silence the buzzer
const unsigned long SHORT_BEEP_MS = 30;
const unsigned long SHORT_START_BEEP_MS = 200;
const unsigned long NORMAL_BEEP_MS = 200;
const unsigned long LONG_BEEP_MS = 600;
const int CHANGE_BEEP_HZ = 2100;
const int SHORT_BEEP_HZ = 2000;  // ~2 kHz = piezo resonance, where it is loudest
const int LOW_BEEP_HZ = 400;
const int HIGH_BEEP_HZ = 800;
const int START_PITCH_BUMP_HZ = 125;
const unsigned long ERROR_BEEP_ON_MS = 80;
const unsigned long ERROR_BEEP_OFF_MS = 120;

void errorBips() {
  for (int i = 0; i < 3; i++) {
    tone(PIN_BUZZER, SHORT_BEEP_HZ);
    delay(ERROR_BEEP_ON_MS);
    noTone(PIN_BUZZER);
    delay(ERROR_BEEP_OFF_MS);
  }
}

void bootBip(int freq) {
  tone(PIN_BUZZER, freq);
  delay(SHORT_BEEP_MS);
  noTone(PIN_BUZZER);
}

void bootBip() {
  bootBip(SHORT_BEEP_HZ);
}

void silenceFinishedBeep() {
  if (bipToneOn && (long)(millis() - bipToneUntil) >= 0) {  // signed compare tolerates millis() wraparound
    noTone(PIN_BUZZER);
    bipToneOn = false;
  }
}

void bip() {
  silenceFinishedBeep();

  // Only react when the countdown second changes.
  if (prevBip == countDownToBip) {
    return;
  }
  prevBip = countDownToBip;

  if (selectbuzz == 0) {
    return;
  }

  int baseFreq;
  bool shortBeeps;
  if (selectbuzz == 1) {
    baseFreq = SHORT_BEEP_HZ;     // identical to the boot beep
    shortBeeps = true;
  } else if (selectbuzz == 2) {   // default
    baseFreq = LOW_BEEP_HZ;
    shortBeeps = false;
  } else {                        // selectbuzz == 3
    baseFreq = HIGH_BEEP_HZ;
    shortBeeps = false;
  }

  int freq = 0;
  unsigned long duration = shortBeeps ? SHORT_BEEP_MS : NORMAL_BEEP_MS;

  if (countDownToBip == 0) {
    freq = baseFreq;
    if (shortBeeps) {
      duration = SHORT_START_BEEP_MS;
    } else {
      freq += START_PITCH_BUMP_HZ;
      duration = LONG_BEEP_MS;
    }
  } else if (countDownToBip == 1 || countDownToBip == 10 || (countDownToBip >= 2 && countDownToBip <= 5)) {
    freq = baseFreq;
  }

  if (freq > 0) {
    tone(PIN_BUZZER, freq);
    bipToneOn = true;
    bipToneUntil = millis() + duration;
  }
}
