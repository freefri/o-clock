void showVersionMessage(const char* sign) {
  strcpy(messageBuf, sign);
  strcat(messageBuf, MSG_VERSION);
  messageText = messageBuf;
  messageColor = Color_orange;
  messageScroll = false;
  messageStart = millis();
  messageUntil = messageStart + 1000;
}

void handleSelectPlusButton() {

  static int lastState = HIGH;
  int state = digitalRead(PIN_BTN_PLUS);

  if (state == LOW && lastState == HIGH) {
    //Serial.println("UP+ ");
    if (mode == MODE_BIP) {
      selectbuzz++;
      if (selectbuzz > 2) selectbuzz = 0;
    } else if (mode == MODE_BRIGHTNESS) {
      int next = brightness + 20;
      brightness = (next > MAX_BRIGHTNESS) ? 10 : next;
      display.setBrightness(brightness);
    } else if (mode == MODE_EDIT) {
      if (submode_editing == 0) {
        submode_editing = EDIT_MODE_HH;
      } else if (submode_editing == EDIT_MODE_HH) {
        editHour(+1);
      } else if (submode_editing == EDIT_MODE_MM) {
        editMinute(+1);
      } else if (submode_editing == EDIT_MODE_SS) {
        editSecond(+1);
      }
    } else if (mode == MODE_CLOCK) {
      messageText = MSG_SPLASH;
      messageColor = Color_blue;
      messageScroll = true;
      messageStart = millis();
      messageUntil = messageStart + DISPLAY_SPLASH_DURING_MS;
    }
  }

  lastState = state;
}
void handleSelectMinusButton() {

  static int lastState = HIGH;
  int state = digitalRead(PIN_BTN_MINUS);

  if (state == LOW && lastState == HIGH) {
    //Serial.println("DOWN- ");
    if (mode == MODE_BIP) {
      if (selectbuzz == 0) selectbuzz = 2;
      else selectbuzz--;
    } else if (mode == MODE_BRIGHTNESS) {
      int prev = brightness - 20;
      brightness = (prev < 10) ? MAX_BRIGHTNESS : prev;
      display.setBrightness(brightness);
    } else if (mode == MODE_EDIT) {
      if (submode_editing == 0) {
        submode_editing = EDIT_MODE_HH;
      } else if (submode_editing == EDIT_MODE_HH) {
        editHour(-1);
      } else if (submode_editing == EDIT_MODE_MM) {
        editMinute(-1);
      } else if (submode_editing == EDIT_MODE_SS) {
        editSecond(-1);
      }
    } else if (mode == MODE_CLOCK) {
      showVersionMessage("- ");
    }
  }

  lastState = state;
}
void handleModeButton() {

  static int lastState = HIGH;
  int state = digitalRead(PIN_BTN_SET);

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
