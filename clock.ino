void updateClock() {

  // Always read the live time from the DS3231 RTC, even while editing, so the
  // clock keeps running on the display. Edits are written straight to the RTC.
  bool h12, pm;  // required by getHour(), unused (24h mode)
  hh = rtc.getHour(h12, pm);
  mm = rtc.getMinute();
  ss = rtc.getSecond();

  if (ss != lastSs) {
    lastSs = ss;

    if (countDownToBip == 0) {
      countDownToBip = secondsToBip - 1;
    } else {
      countDownToBip--;
    }
  }
}

// EDIT mode nudges the RTC directly (delta = +1 or -1), so the clock keeps
// running on the display while you set it.
void editHour(int delta) {
  bool h12, pm;
  int v = (int)rtc.getHour(h12, pm) + delta;
  if (v > 23) v = 0;
  if (v < 0) v = 23;
  rtc.setHour(v);  // 0..23 (24h mode set in setup)
}

void editMinute(int delta) {
  int v = (int)rtc.getMinute() + delta;
  if (v > 59) v = 0;
  if (v < 0) v = 59;
  rtc.setMinute(v);
}

void editSecond(int delta) {
  int v = (int)rtc.getSecond() + delta;
  if (v > 59) v = 0;
  if (v < 0) v = 59;
  rtc.setSecond(v);
  countDownToBip = 60 - v;  // keep the long bip aligned to second 0
  lastSs = v;
}
