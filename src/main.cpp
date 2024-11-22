#include "board.h"

void setup() { bsp_init(); }

void loop() {
  buttons_loop();
  dtof_loop();
  // delay(20);
}