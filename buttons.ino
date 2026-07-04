const unsigned long DEBOUNCE_MS = 40;

void showVersionMessage(const char* sign) {
  strcpy(messageBuf, sign);
  strcat(messageBuf, MSG_VERSION);
  messageText = messageBuf;
  messageColor = Color_orange;
  messageScroll = false;
  messageStart = millis();
  messageUntil = messageStart + 1000;
}

bool debouncedButtonPress(byte pin, int &lastState, unsigned long &lastPressAt) {
  int state = digitalRead(pin);
  bool edge = (state == LOW && lastState == HIGH);
  lastState = state;
  if (edge && (millis() - lastPressAt) > DEBOUNCE_MS) {
    lastPressAt = millis();
    return true;
  }
  return false;
}

void handleSelectPlusButton() {
  static int lastState = HIGH;
  static unsigned long lastPressAt = 0;
  if (!debouncedButtonPress(PIN_BTN_PLUS, lastState, lastPressAt)) return;

    bootBip(CHANGE_BEEP_HZ);
    if (mode == MODE_BIP) {
      selectbuzz++;
      if (selectbuzz > 3) selectbuzz = 0;
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
      strcpy(messageBuf, "+");
      strcat(messageBuf, MSG_SPLASH);   // leading + is drawn orange by the scroller
      messageText = messageBuf;
      messageColor = Color_blue;
      messageScroll = true;
      messageStart = millis();
      messageUntil = messageStart + DISPLAY_SPLASH_DURING_MS;
    }
}

void handleSelectMinusButton() {
  static int lastState = HIGH;
  static unsigned long lastPressAt = 0;
  if (!debouncedButtonPress(PIN_BTN_MINUS, lastState, lastPressAt)) return;

    bootBip(CHANGE_BEEP_HZ);
    if (mode == MODE_BIP) {
      if (selectbuzz == 0) selectbuzz = 3;
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

void handleModeButton() {
    static int lastState = HIGH;
    static unsigned long lastPressAt = 0;
    if (!debouncedButtonPress(PIN_BTN_SET, lastState, lastPressAt)) return;

    bootBip();
    if (mode == MODE_EDIT && submode_editing > 0) {
      submode_editing++;
      if (submode_editing > EDIT_MODE_SS) {
        submode_editing = 0;
        mode = MODE_CLOCK;
      }
    } else {
      mode++;
    }
    if (mode > MODE_LAST) mode = 0;
}
