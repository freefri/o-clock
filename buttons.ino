void handleSelectPlusButton() {

  static int lastState = HIGH;
  int state = digitalRead(Pinc);

  if (state == LOW && lastState == HIGH) {

    //Serial.println("UP+ ");
    if (mode == MODE_BIP) {
      selectbuzz++;
      if (selectbuzz > 2) selectbuzz = 0;
    } else if (mode == MODE_BRIGHTNESS) {
      brightness += 10;
      if (brightness > 50) brightness = 10;
      display.setBrightness(brightness);
    } else if (mode == MODE_EDIT) {
      if (submode_editing == 0) {
        submode_editing = EDIT_MODE_HH;
        loadEditTimeFromRtc();
      } else if (submode_editing == EDIT_MODE_HH) {
        clock_hh++;
        if (clock_hh > 23) {
          clock_hh = 0;
        }
      } else if (submode_editing == EDIT_MODE_MM) {
        clock_mm++;
        if (clock_mm > 59) {
          clock_mm = 0;
        }
      } else if (submode_editing == EDIT_MODE_SS) {
        clock_ss++;
        //Serial.println("clock_ss++");
        if (clock_ss > 59) {
          clock_ss = 0;
        }
        countDownToBip = 60 - clock_ss;
        lastSs = clock_ss;
      }
      //Serial.print("mode EDIT -> submode -> ");
      //Serial.println(submode_editing);
    }
  }

  lastState = state;
}
void handleSelectMinusButton() {

  static int lastState = HIGH;
  int state = digitalRead(Pdec);

  if (state == LOW && lastState == HIGH) {

    //Serial.println("DOWN- ");
    if (mode == MODE_BIP) {
      if (selectbuzz == 0) selectbuzz = 2;
      else selectbuzz--;
    } else if (mode == MODE_BRIGHTNESS) {
      brightness -= 10;
      if (brightness < 10) brightness = 50;
      display.setBrightness(brightness);
    } else if (mode == MODE_EDIT) {
      if (submode_editing == 0) {
        submode_editing = EDIT_MODE_HH;
        loadEditTimeFromRtc();
      } else if (submode_editing == EDIT_MODE_HH) {
        if (clock_hh == 0) {
          clock_hh = 23;
        } else {
          clock_hh--;
        }
      } else if (submode_editing == EDIT_MODE_MM) {
        if (clock_mm == 0) {
          clock_mm = 59;
        } else {
          clock_mm--;
        }
      } else if (submode_editing == EDIT_MODE_SS) {
        if (clock_ss == 0) {
          clock_ss = 59;
        } else {
          clock_ss--;
        }
        //Serial.println("clock_ss--");
        countDownToBip = 60 - clock_ss;
        lastSs = clock_ss;
      }
      //Serial.print("mode EDIT -> submode -> ");
      //Serial.println(submode_editing);
    }
  }

  lastState = state;
}
void handleModeButton() {

  static int lastState = HIGH;
  int state = digitalRead(Pset);

  if (state == LOW && lastState == HIGH) {
    if (mode == MODE_EDIT && submode_editing > 0) {
      submode_editing++;
      if (submode_editing > EDIT_MODE_SS) {
        submode_editing = 0;
        mode = MODE_CLOCK;
        commitEditTimeToRtc();
      }
      //Serial.print("submode -> ");
      //Serial.println(submode_editing);
    } else {
      //Serial.println("change mode ");
      mode++;
    }
    if (mode > MODE_LAST) mode = 0;
    //Serial.print("current mode -> ");
    //Serial.println(mode);
  }

  lastState = state;
}
