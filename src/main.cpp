#include <SparkFun_TMF882X_Library.h>
#include <TFT_eSPI.h>
#include <Wire.h>

// 引脚定义
#define I2C_SDA 16
#define I2C_SCL 17

arduino::MbedI2C Wire1(I2C_SDA, I2C_SCL);

// TMF882X 和 TFT_eSPI 的实例
SparkFun_TMF882X myTMF882X;
static struct tmf882x_msg_meas_results myResults = {0};
TFT_eSPI tft = TFT_eSPI();

TFT_eSprite sprite = TFT_eSprite(&tft);

void setup() {
  // 初始化串口
  Serial.begin(115200);
  Serial.println("\nSystem Starting...");

  Serial.println("I2C initialized.");

  // 初始化 TFT_eSPI
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.drawString("RP2040 & TMF8821", 10, 10);
  Wire1.begin();
  // 初始化 TMF882X
  if (!myTMF882X.begin(Wire1)) {
    Serial.println(
        "Error - The TMF882X failed to initialize - is the board connected?");
    while (1)
      ; // 错误时卡住程序
  } else {
    Serial.println("TMF882X started.");
    tft.drawString("TMF882X started.", 10, 40);
  }

  sprite.createSprite(tft.width(), tft.height());
  sprite.fillSprite(TFT_BLACK);
  sprite.setTextColor(TFT_WHITE, TFT_BLACK);
}

void loop() {
  delay(50);

  // 清除 Sprite 内容
  sprite.fillSprite(TFT_BLACK);

  // 显示标题（固定显示）
  sprite.setTextSize(2);
  sprite.setTextColor(TFT_WHITE, TFT_BLACK);
  sprite.drawString("RP2040 & TMF8821", 10, 0);

  // 获取测量数据
  if (myTMF882X.startMeasuring(myResults)) {
    Serial.println("Measurement:");
    Serial.print("     Result Number: ");
    Serial.print(myResults.result_num);
    Serial.print("  Number of Results: ");
    Serial.println(myResults.num_results);

    sprite.setCursor(10, 30); // 设置动态内容的起始位置
    sprite.setTextSize(1);

    // 显示测量结果到屏幕
    for (int i = 0; i < myResults.num_results; ++i) {
      // 打印到串口
      Serial.print("       conf: ");
      Serial.print(myResults.results[i].confidence);
      Serial.print(" distance mm: ");
      Serial.print(myResults.results[i].distance_mm);
      Serial.print(" channel: ");
      Serial.print(myResults.results[i].channel);
      Serial.print(" sub_capture: ");
      Serial.println(myResults.results[i].sub_capture);

      // 显示到 Sprite
      sprite.print("Conf: ");
      sprite.print(myResults.results[i].confidence);
      sprite.print(" Dist: ");
      sprite.print(myResults.results[i].distance_mm);
      sprite.println(" mm");
    }

    Serial.print("     photon: ");
    Serial.print(myResults.photon_count);
    Serial.print(" ref photon: ");
    Serial.print(myResults.ref_photon_count);
    Serial.print(" ALS: ");
    Serial.println(myResults.ambient_light);
    Serial.println();

    // 显示光子计数和环境光信息到 Sprite
    sprite.print("Photon: ");
    sprite.print(myResults.photon_count);
    sprite.print(" ALS: ");
    sprite.println(myResults.ambient_light);
  } else {
    Serial.println("Measurement failed.");
    sprite.setCursor(10, 30);
    sprite.setTextSize(2);
    sprite.drawString("Measurement failed!", 10, 30);
  }

  sprite.pushSprite(0, 0);
}




// class pin_cfg:
//     yellow_led = 20
//     blue_led = 21
//     green_led = 22
//     red_led = 26
    
//     buzzer = 19
//     mic = 27
    
//     i2c0_scl = 17
//     i2c0_sda = 16
    
//     i2c1_scl = 15
//     i2c1_sda = 14

//     spi1_mosi = 15
//     spi1_sck = 14
//     spi1_dc = 12
//     spi1_rstn = 16
//     spi1_cs = 13

//     adc0 = 26
//     adc1 = 27

//     k1 = 12
//     k2 = 13

//     pot = 28