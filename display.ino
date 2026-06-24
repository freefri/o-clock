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
