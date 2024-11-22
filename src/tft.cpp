#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

void tft_init() {
  // 初始化 TFT_eSPI
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.drawString("RP2040 & TMF8821", 10, 10);

  sprite.createSprite(tft.width(), tft.height());
  sprite.setTextSize(2);
}

// Helper function to map distance to color
uint16_t getColorForDistance(uint32_t distance) {
  if (distance < 100)
    return TFT_GREEN;
  else if (distance < 200)
    return TFT_YELLOW;
  else if (distance < 300)
    return TFT_ORANGE;
  else
    return TFT_RED;
}
