void bip() {
  if (prevBip == countDownToBip) {
    return;
  }
  prevBip = countDownToBip;
  if (selectbuzz == 0) {
    return;
  }
  int tone1 = 400;
  tone1 = tone1 * selectbuzz;
  int tone2 = 125;
  if (countDownToBip == 1) {
    tone(Buzz, tone1);
  } else if (countDownToBip == 10) {
    tone(Buzz, tone1);
  } else if (countDownToBip == 0) {
    tone(Buzz, tone1 + tone2);
    delay(400);
  } else if (countDownToBip <= 5 && countDownToBip > 1) {
    tone(Buzz, tone1);
  }
  delay(200);
  noTone(Buzz);
}
