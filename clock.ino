void updateClock() {

  if (mode == MODE_EDIT && submode_editing > 0) {
    // While editing, display the values being set instead of the live time,
    // so each button press is visible (and the RTC isn't read mid-edit).
    hh = clock_hh;
    mm = clock_mm;
    ss = clock_ss;
  } else {
    // Read the live time from the DS3231 RTC.
    bool h12;  // 12h format
    bool pm;   // flag pm
    hh = rtc.getHour(h12, pm);
    mm = rtc.getMinute();
    ss = rtc.getSecond();
  }

  if (ss != lastSs) {
    lastSs = ss;

    if (countDownToBip == 0) {
      countDownToBip = secondsToBip - 1;
    } else {
      countDownToBip--;
    }
  }
}

void loadEditTimeFromRtc() {
  // Start editing from the current RTC time, so you adjust "now" not 12:00.
  if (rtcAvailable) {
    bool h12, pm;
    clock_hh = rtc.getHour(h12, pm);
    clock_mm = rtc.getMinute();
    clock_ss = rtc.getSecond();
  }
}

void commitEditTimeToRtc() {
  // Write the edited time to the RTC when leaving EDIT mode.
  if (rtcAvailable) {
    rtc.setHour(clock_hh);    // 0..23 (24h mode set in setup)
    rtc.setMinute(clock_mm);
    rtc.setSecond(clock_ss);
  }
}
