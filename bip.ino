unsigned long bipToneUntil = 0;  // millis() time at which to silence the buzzer

void errorBips() {
  for (int i = 0; i < 3; i++) {
    tone(PIN_BUZZER, 2000);
    delay(80);
    noTone(PIN_BUZZER);
    delay(120);
  }
}

void bip() {
  // Silence the current beep once its duration has elapsed. Non-blocking, so
  // it no longer stalls the loop (the delay()s were what made the beep
  // intervals uneven). Signed compare tolerates millis() wraparound.
  if (bipToneOn && (long)(millis() - bipToneUntil) >= 0) {
    noTone(PIN_BUZZER);
    bipToneOn = false;
  }

  // Only react when the countdown second changes.
  if (prevBip == countDownToBip) {
    return;
  }
  prevBip = countDownToBip;

  if (selectbuzz == 0) {
    return;
  }

  int tone1 = 400 * selectbuzz;
  int tone2 = 125;
  int freq = 0;
  unsigned long dur = 200;  // short beep

  if (countDownToBip == 1) {
    freq = tone1;
  } else if (countDownToBip == 10) {
    freq = tone1;
  } else if (countDownToBip == 0) {
    freq = tone1 + tone2;  // long, higher beep on second 0
    dur = 600;
  } else if (countDownToBip <= 5 && countDownToBip > 1) {
    freq = tone1;
  }

  if (freq > 0) {
    tone(PIN_BUZZER, freq);
    bipToneOn = true;
    bipToneUntil = millis() + dur;
  }
}
