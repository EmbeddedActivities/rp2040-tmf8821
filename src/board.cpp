
#include <Wire.h>
#include "board.h"

arduino::MbedI2C Wire1(I2C0_SDA, I2C0_SCL);

void bsp_init() {
  Serial.begin(115200);
  Serial.println("\nSystem Starting...");
  Serial.println("I2C initialized.");
  Wire1.begin();
  tft_init();
  dtof_init();
  buttons_init();
}