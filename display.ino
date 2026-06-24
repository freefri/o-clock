void drawClock(int hh, int mm, int ss) {

  display.clear();

  const int W = 4; // digit width

  int x = 0;

  // HH
  x += Digit2Display(hh / 10, x, 0, Color_hhmm);
  x += Digit2Display(hh % 10, x, 0, Color_hhmm);

  x += ColonBlinking(x, 0, Color_hhmm);

  // MM
  x += Digit2Display(mm / 10, x, 0, Color_hhmm);
  x += 1;
  x += Digit2Display(mm % 10, x, 0, Color_hhmm);

  x += ColonBlinking(x, 0, Color_hhmm);

  // SS
  x += Digit2Display(ss / 10, x, 0, Color_hhmm);
  x += 1;
  x += Digit2Display(ss % 10, x, 0, Color_hhmm);

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
void Chars2Display(byte d1, byte d2, byte d3, byte d4, byte d5, byte d6, byte mode, byte cs) {

  display.clear();

  // column positions for digits
  int xPos[6] = {0, 4, 10, 14, 20, 24};
  int yPos = 0;

  Digit2Display(d1, xPos[0], yPos, Color_hhmm);
  Digit2Display(d2, xPos[1], yPos, Color_hhmm);

  Digit2Display(d3, xPos[2], yPos, Color_hhmm);
  Digit2Display(d4, xPos[3], yPos, Color_hhmm);

  Digit2Display(d5, xPos[4], yPos, Color_hhmm);
  Digit2Display(d6, xPos[5], yPos, Color_hhmm);

  display.show();
}

int asciiToIndex(byte chr) {

  // numeric digits
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

  // fallback
  return 255;
}

int Digit2Display(byte chr, int xOffset, int yOffset, uint32_t color) {

  int glyph = asciiToIndex(chr);

  if (glyph == 255) return;

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
