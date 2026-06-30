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
      } else if (submode_editing == EDIT_MODE_HH) {
        clock_hh++;
        if (clock_hh > 24) {
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
      }
      //Serial.print("mode EDIT -> submode -> ");
      //Serial.println(submode_editing);
    }
  }

  lastState = state;
}
void handleSelectMinusButton() {

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
      } else if (submode_editing == EDIT_MODE_HH) {
        clock_hh++;
        if (clock_hh > 24) {
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
