void drawMessage(const char* text, uint32_t color) {
  display.clear();
  DisplayText(text, 0, 0, color);
  display.show();
}

// Rendered width in columns, matching DisplayText's per-character advance.
int textWidth(const char* text) {
  int w = 0;
  while (*text) {
    if (*text == ':') w += 3;
    else if (asciiToIndex(*text) == 255) w += 1;  // space / unknown
    else w += 5;                                   // 4px glyph + 1px spacing
    text++;
  }
  return w > 0 ? w - 1 : 0;
}

void drawScrollingMessage(const char* text, uint32_t color, unsigned long startMs) {
  const int SCROLL_COLUMNS_PER_SECOND = 15;
  const unsigned long SCROLL_INITIAL_HOLD_MS = 400;
  int width = textWidth(text);
  int travel = WIDTH + width;
  unsigned long elapsed = millis() - startMs;
  elapsed = (elapsed > SCROLL_INITIAL_HOLD_MS) ? (elapsed - SCROLL_INITIAL_HOLD_MS) : 0;
  int elapsedPx = elapsed * SCROLL_COLUMNS_PER_SECOND / 1000;
  int x = -(elapsedPx % travel);
  if (x < -width) x += travel;

  display.clear();
  DisplayText(text, x, 0, color);
  display.show();
}

void drawClock(int hh, int mm, int ss) {

  display.clear();

  if (!is24hours) {
    if (hh > 12) hh -= 12;
  }

  int x = (hh >= 10) ? 0 : 4;

  uint32_t colorHH;
  if (submode_editing == EDIT_MODE_HH) {
    colorHH = Color_blue;
  } else {
    colorHH = Color_white;
  }
  uint32_t colorMM;
  if (submode_editing == EDIT_MODE_MM) {
    colorMM = Color_blue;
  } else {
    colorMM = Color_white;
  }
  uint32_t colorSS;
  if (submode_editing == EDIT_MODE_SS) {
    colorSS = Color_blue;
  } else {
    if (countDownToBip == 1) {
      colorSS = Color_red;
    } else if (countDownToBip == 0) {
      colorSS = Color_green;
    } else if (countDownToBip <= 5 && countDownToBip > 1) {
      colorSS = Color_orange;
    } else {
      colorSS = Color_white;
    }
  }

  // HH — hide the leading zero for single-digit hours
  if (hh >= 10) {
    x += Digit2Display(hh / 10, x, 0, colorHH);
  }
  x += Digit2Display(hh % 10, x, 0, colorHH);

  x += ColonBlinking(x, 0, Color_white);

  // minutes
  x += Digit2Display(mm / 10, x, 0, colorMM);
  x += 1;
  x += Digit2Display(mm % 10, x, 0, colorMM);

  x += ColonBlinking(x, 0, Color_white);

  // seconds
  x += Digit2Display(ss / 10, x, 0, colorSS);
  x += 1;
  x += Digit2Display(ss % 10, x, 0, colorSS);

  display.show();
}

int mapXYtoSnakeStrip(int x, int y) {

  int index;

  if (x % 2 == 0) {
    index = (x * HEIGHT) + y;
  } else {
    index = (x * HEIGHT) + (HEIGHT - 1 - y);
  }

  return index;
}

int ColonBlinking(int xOffset, int yOffset, uint32_t color) {
  if (colonOn) {
    return Colon(xOffset, yOffset, color);
  } else {
    return 3;
  }
}

int Colon(int xOffset, int yOffset, uint32_t color) {

  const uint8_t COLON[8][3] = {
    {0,0,0},
    {0,0,0},
    {0,1,0},
    {0,0,0},
    {0,0,0},
    {0,1,0},
    {0,0,0},
    {0,0,0}
  };

  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 3; x++) {

      if (COLON[y][x] == 1) {

        int px = xOffset + x;
        int py = yOffset + y;

        if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
          int ledIndex = mapXYtoSnakeStrip(px, py);
          display.setPixelColor(ledIndex, color);
        }
      }
    }
  }

  return 3;
}

int asciiToIndex(byte chr) {

  // accepts either an already-decoded glyph index (0..9) or an ASCII character
  if (chr <= 9) {
    return chr;
  }
  // digits
  if (chr >= '0' && chr <= '9') {
    return chr - '0';
  }
  // uppercase
  if (chr >= 'A' && chr <= 'Z') {
    return 10 + (chr - 'A');
  }
  // lowercase
  if (chr >= 'a' && chr <= 'z') {
    return 10 + (chr - 'a');
  }
  if (chr == '+') return 36;
  if (chr == '-') return 37;
  if (chr == '.') return 38;

  return 255;  // unknown character
}

int Digit2Display(byte chr, int xOffset, int yOffset, uint32_t color) {

  int glyph = asciiToIndex(chr);

  if (glyph == 255) {
    return 0;
  }

  const int DIGIT_W = 4;
  const int DIGIT_H = 8;

  for (int i = 0; i < 32; i++) {

    uint8_t glyphValue = pgm_read_byte(&(GLYPH[glyph][i]));
    if (glyphValue == 1) {

      int x = xOffset + (i % DIGIT_W);
      int y = yOffset + (i / DIGIT_W);

      if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {

        int ledIndex = mapXYtoSnakeStrip(x, y);
        display.setPixelColor(ledIndex, color);

      }
    }
  }
  return 4; // return width plus separator
}
int DisplayText(const char* text, int xOffset, int yOffset, uint32_t color) {

  int x = xOffset;

  while (*text) {

    if (*text == ':') {
      x += Colon(x - 1, yOffset, color);
    } else {
      x += Digit2Display(*text, x, yOffset, color);
      x += 1;   // one column of spacing
    }

    text++;
  }

  return x - xOffset - 1;
}
