#include <OneButton.h>
#include <conf.h>

const int THRESHOLD = 512;  // 阈值为 512（代表 0.5 映射）
const float MAX_ADC_VALUE = 1024.0;
const float DEAD_ZONE = 0.1;  // 静止区间设置为 -0.1 到 0.1

// 映射函数：将 0-1024 映射到 -1 到 1
// float mapToRange(int value) {
//   return (value - (MAX_ADC_VALUE / 2)) / (MAX_ADC_VALUE / 2);
// }

// 判断是否在静止区间
// bool isInDeadZone(float value) {
//   return (value > -DEAD_ZONE && value < DEAD_ZONE);
// }


// 映射函数：将 0-1024 映射到 -1 到 1 （X轴反向映射）
float mapToRangeX(int value) {
  return (value - (MAX_ADC_VALUE / 2)) / (MAX_ADC_VALUE / 2);
}

// 映射函数：将 0-1024 映射到 -1 到 1 （Y轴反向映射）
float mapToRangeY(int value) {
  return ((MAX_ADC_VALUE - value) - (MAX_ADC_VALUE / 2)) / (MAX_ADC_VALUE / 2);
}

// 判断是否在静止区间
bool isInDeadZone(float value) {
  return (value > -DEAD_ZONE && value < DEAD_ZONE);
}

OneButton ButtonA;
OneButton ButtonB;
OneButton ButtonStart;
OneButton ButtonSelect;

void buttons_init() {

  pinMode(JOYSTICK_Y_PIN, INPUT);  
  pinMode(JOYSTICK_X_PIN, INPUT);  

  ButtonA.setup(ButtonA_PIN, INPUT_PULLUP, true);
  ButtonB.setup(ButtonB_PIN, INPUT_PULLUP, true);
  ButtonStart.setup(ButtonStart_PIN, INPUT_PULLUP, true);
  ButtonSelect.setup(ButtonSelect_PIN, INPUT_PULLUP, true);

  ButtonA.attachClick([]() { Serial.println("Button A clicked!"); });
  ButtonB.attachClick([]() { Serial.println("Button B clicked!"); });
  ButtonStart.attachClick([]() { Serial.println("Button Start clicked!"); });
  ButtonSelect.attachClick([]() { Serial.println("Button Select clicked!"); });
}

void buttons_loop() {
  ButtonA.tick();
  ButtonB.tick();
  ButtonStart.tick();
  ButtonSelect.tick();

// 读取 ADC 值
  int x = analogRead(JOYSTICK_X_PIN);  
  int y = analogRead(JOYSTICK_Y_PIN); 
  
  // 映射到 -1 到 1 的范围
  float mappedX = mapToRangeX(x); 
  float mappedY = mapToRangeY(y);  
  
  // 输出映射值
  // Serial.print("Mapped X: ");
  // Serial.print(mappedX);
  // Serial.print(", Mapped Y: ");
  // Serial.println(mappedY);

  // 判断是否处于静止区间
  if (isInDeadZone(mappedX)) {
    Serial.println("X is in dead zone (no action)");
  } else if (mappedX > 0.5) {
    Serial.println("X exceeds positive threshold");
  } else if (mappedX < -0.5) {
    Serial.println("X exceeds negative threshold");
  }

  if (isInDeadZone(mappedY)) {
    Serial.println("Y is in dead zone (no action)");
  } else if (mappedY > 0.5) {
    Serial.println("Y exceeds positive threshold");
  } else if (mappedY < -0.5) {
    Serial.println("Y exceeds negative threshold");
  }

}