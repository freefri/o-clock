/*** RTL alternative */
void updateClock() {

  if (rtcAvailable) {
    // ===== USE RTC =====
    hh = rtc.getHour(h12, pm);
    mm = rtc.getMinute();
    ss = rtc.getSecond();
  } else {
    // ===== USE INTERNAL CLOCK =====
    unsigned long now = millis();

    if (now - lastMillis >= 1000) {
      lastMillis += 1000;

      clock_ss++;

      if (clock_ss >= 60) {
        clock_ss = 0;
        clock_mm++;
      }

      if (clock_mm >= 60) {
        clock_mm = 0;
        clock_hh++;
      }

      if (clock_hh >= 24) {
        clock_hh = 0;
      }
    }

    hh = clock_hh;
    mm = clock_mm;
    ss = clock_ss;
  }

  // todo for debug
  static unsigned long lastPrint = 0;

  if (millis() - lastPrint >= 1000) {
    lastPrint = millis();
  }
}
