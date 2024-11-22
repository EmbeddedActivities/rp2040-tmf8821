#include "conf.h"
#include "tft.h"
#include <SparkFun_TMF882X_Library.h>
#include <TFT_eSPI.h>
#include <Wire.h>

const int matrixSize = 3;

// 平面拟合函数
void fitPlaneAndCalculateAngle(float Z[matrixSize][matrixSize], float &a,
                               float &b, float &d, float &angle) {
  // 展开 x, y, z 点
  float x_coords[matrixSize * matrixSize];
  float y_coords[matrixSize * matrixSize];
  float z_coords[matrixSize * matrixSize];
  int index = 0;

  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      x_coords[index] = j - 1;   // x 坐标: -1, 0, 1
      y_coords[index] = i - 1;   // y 坐标: -1, 0, 1
      z_coords[index] = Z[i][j]; // z 坐标
      index++;
    }
  }

  // 构造 A 矩阵和 b 向量
  float ATA[3][3] = {0}; // ATA 矩阵 (3x3)
  float ATb[3] = {0};    // ATb 向量 (3x1)
  for (int i = 0; i < matrixSize * matrixSize; i++) {
    float x = x_coords[i];
    float y = y_coords[i];
    float z = z_coords[i];

    ATA[0][0] += x * x;
    ATA[0][1] += x * y;
    ATA[0][2] += x;

    ATA[1][0] += y * x;
    ATA[1][1] += y * y;
    ATA[1][2] += y;

    ATA[2][0] += x;
    ATA[2][1] += y;
    ATA[2][2] += 1;

    ATb[0] += x * z;
    ATb[1] += y * z;
    ATb[2] += z;
  }

  // 计算 (ATA)^(-1) 和结果
  float det = ATA[0][0] * (ATA[1][1] * ATA[2][2] - ATA[1][2] * ATA[2][1]) -
              ATA[0][1] * (ATA[1][0] * ATA[2][2] - ATA[1][2] * ATA[2][0]) +
              ATA[0][2] * (ATA[1][0] * ATA[2][1] - ATA[1][1] * ATA[2][0]);

  if (det == 0) {
    Serial.println("无法拟合平面，矩阵不可逆");
    a = b = d = angle = NAN; // 返回无效值
    return;
  }

  // 逆矩阵 ATA^(-1)
  float invATA[3][3];
  invATA[0][0] = (ATA[1][1] * ATA[2][2] - ATA[1][2] * ATA[2][1]) / det;
  invATA[0][1] = (ATA[0][2] * ATA[2][1] - ATA[0][1] * ATA[2][2]) / det;
  invATA[0][2] = (ATA[0][1] * ATA[1][2] - ATA[0][2] * ATA[1][1]) / det;
  invATA[1][0] = (ATA[1][2] * ATA[2][0] - ATA[1][0] * ATA[2][2]) / det;
  invATA[1][1] = (ATA[0][0] * ATA[2][2] - ATA[0][2] * ATA[2][0]) / det;
  invATA[1][2] = (ATA[0][2] * ATA[1][0] - ATA[0][0] * ATA[1][2]) / det;
  invATA[2][0] = (ATA[1][0] * ATA[2][1] - ATA[1][1] * ATA[2][0]) / det;
  invATA[2][1] = (ATA[0][1] * ATA[2][0] - ATA[0][0] * ATA[2][1]) / det;
  invATA[2][2] = (ATA[0][0] * ATA[1][1] - ATA[0][1] * ATA[1][0]) / det;

  // 计算 a, b, d
  a = invATA[0][0] * ATb[0] + invATA[0][1] * ATb[1] + invATA[0][2] * ATb[2];
  b = invATA[1][0] * ATb[0] + invATA[1][1] * ATb[1] + invATA[1][2] * ATb[2];
  d = invATA[2][0] * ATb[0] + invATA[2][1] * ATb[1] + invATA[2][2] * ATb[2];

  // 法向量
  float nx = a, ny = b, nz = -1;

  // 计算与 z 轴的夹角
  float magnitude = sqrt(nx * nx + ny * ny + nz * nz);
  float cosTheta = abs(nz) / magnitude;
  angle = acos(cosTheta) * 180.0 / PI; // 转为角度
}

extern TFT_eSPI tft;
extern TFT_eSprite sprite;
extern arduino::MbedI2C Wire1;

const uint8_t CONFIDENCE_THRESHOLD = 200;
const int GRID_SIZE = 3;
const int SCREEN_SIZE = 240;
const int CELL_SIZE = SCREEN_SIZE / GRID_SIZE;


const int centerX = 120; // 矩形中心 X 坐标
const int centerY = 120; // 矩形中心 Y 坐标
const int rectWidth = 100; // 矩形宽度
const int rectHeight = 100; // 矩形高度

SparkFun_TMF882X dToF;
// static struct tmf882x_msg_meas_results myResults = {0};

void onMeasurementCallback(struct tmf882x_msg_meas_results *myResults) {

  // 输入的 Z 值矩阵
  float Z[matrixSize][matrixSize] = {0};

  for (int i = 0; i < myResults->num_results; ++i) {
    int channel = myResults->results[i].channel;
    float distance = myResults->results[i].distance_mm;

    // 将 channel 映射到矩阵索引
    int row = (channel - 1) / matrixSize;
    int col = (channel - 1) % matrixSize;

    // 存入矩阵
    Z[row][col] = distance;
  }

  // 平面参数
  float a, b, d, angle;

  // 调用拟合函数
  fitPlaneAndCalculateAngle(Z, a, b, d, angle);
  Serial.print("a: ");
  Serial.println(a);
  Serial.print("b: ");
  Serial.println(b);
  Serial.print("d: ");
  Serial.println(d);
  Serial.print("angle: ");
  Serial.println(angle);

  // 填充背景
  sprite.fillSprite(TFT_BLACK); // 清空背景


  // / 转换角度为弧度
  float radY = radians(angle);

  // 计算矩形的四个顶点（从 Y 轴上方看 Z 轴的角度）
  int x1 = centerX + (-rectWidth / 2);
  int y1 = centerY + (-rectHeight / 2) * cos(radY);

  int x2 = centerX + (rectWidth / 2);
  int y2 = centerY + (-rectHeight / 2) * cos(radY);

  int x3 = centerX + (rectWidth / 2);
  int y3 = centerY + (rectHeight / 2) * cos(radY);

  int x4 = centerX + (-rectWidth / 2);
  int y4 = centerY + (rectHeight / 2) * cos(radY);

  // 绘制矩形的两个三角形
  sprite.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_RED);
  sprite.fillTriangle(x1, y1, x3, y3, x4, y4, TFT_RED);

  // 绘制坐标轴
  sprite.drawLine(centerX - 50, centerY, centerX + 50, centerY, TFT_WHITE); // X 轴
  sprite.drawLine(centerX, centerY - 50, centerX, centerY + 50, TFT_WHITE); // Z 轴

  // 将 Sprite 推送到屏幕
  sprite.pushSprite(0, 0);

  // // 转换角度为弧度
  // float radX = radians(angle);

  // // 计算矩形的四个顶点（从 Y 轴视角看 X-Z 平面）
  // int x1 = centerX + (-rectWidth / 2);
  // int z1 = centerY + (-rectHeight / 2) * cos(radX);

  // int x2 = centerX + (rectWidth / 2);
  // int z2 = centerY + (-rectHeight / 2) * cos(radX);

  // int x3 = centerX + (rectWidth / 2);
  // int z3 = centerY + (rectHeight / 2) * cos(radX);

  // int x4 = centerX + (-rectWidth / 2);
  // int z4 = centerY + (rectHeight / 2) * cos(radX);

  // // 绘制矩形的两个三角形
  // sprite.fillTriangle(x1, z1, x2, z2, x3, z3, TFT_RED);
  // sprite.fillTriangle(x1, z1, x3, z3, x4, z4, TFT_RED);

  // // 绘制坐标轴
  // sprite.drawLine(centerX - 50, centerY, centerX + 50, centerY, TFT_WHITE); // X 轴
  // sprite.drawLine(centerX, centerY - 50, centerX, centerY + 50, TFT_WHITE); // Z 轴

  // // 将 Sprite 推送到屏幕
  // sprite.pushSprite(0, 0);

  // Serial.println("num_results:");
  // Serial.println(myResults->num_results);

  // Serial.println("Measurement:");
  // Serial.print("     Result Number: ");
  // Serial.print(myResults->result_num);
  // Serial.print("  Number of Results: ");
  // Serial.println(myResults->num_results);
  // for (int i = 0; i < myResults->num_results; ++i) {
  //   // 打印到串口
  //   Serial.print("       conf: ");
  //   Serial.print(myResults->results[i].confidence);
  //   Serial.print(" distance mm: ");
  //   Serial.print(myResults->results[i].distance_mm);
  //   Serial.print(" channel: ");
  //   Serial.print(myResults->results[i].channel);
  //   Serial.print(" sub_capture: ");
  //   Serial.println(myResults->results[i].sub_capture);
  // }

  // int validIndex = 0; // 用于跟踪有效的区域索引
  // for (int i = 0; i < myResults->num_results; ++i) {

  //   // 跳过无效区域（区域 9 和 18），但是不影响有效区域的位置
  //   // if (myResults->results[i].channel == 9 ||
  //   myResults->results[i].channel
  //   // == 18)
  //   // {
  //   //     continue; // 直接跳过区域 9 和 18
  //   // }

  //   // 计算有效区域的行列位置
  //   int row = validIndex / GRID_SIZE;
  //   int col = validIndex % GRID_SIZE;
  //   validIndex++; // 有效区域索引递增
  //   if (myResults->results[i].confidence >= CONFIDENCE_THRESHOLD) {
  //     uint16_t color =
  //     getColorForDistance(myResults->results[i].distance_mm);

  //     // 绘制方格
  //     sprite.fillRect(col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE,
  //                     color);

  //     // 显示距离值
  //     char distanceText[10];
  //     sprintf(distanceText, "%d", myResults->results[i].distance_mm);
  //     int textX = col * CELL_SIZE + CELL_SIZE / 2 -
  //                 sprite.textWidth(distanceText) / 2;  // 居中
  //     int textY = row * CELL_SIZE + CELL_SIZE / 2 - 8; // 字体高度约为 16
  //     sprite.setTextColor(TFT_WHITE, color);
  //     sprite.drawString(distanceText, textX, textY);
  //   }
  // }

  // // 将绘制的内容推送到屏幕
  sprite.pushSprite(0, 0);
}

void dtof_init() {
  // 初始化串口
  // dToF.setCurrentSPADMap(TMF8X2X_COM_SPAD_MAP_ID__spad_map_id__map_no_7);
  dToF.setCurrentSPADMap(TMF8X2X_COM_SPAD_MAP_ID__spad_map_id__map_no_3);
  // 初始化 TMF882X
  if (!dToF.begin(Wire1)) {
    Serial.println(
        "Error - The TMF882X failed to initialize - is the board connected?");
    while (1)
      ; // 错误时卡住程序
  } else {
    Serial.println("TMF882X started.");
    tft.drawString("TMF882X started.", 10, 40);
  }

  // set our callback function in the library.
  dToF.setMeasurementHandler(onMeasurementCallback);

  // Set our delay between samples in the processing loop  - 1 second - note
  // it's in ms
  dToF.setSampleDelay(20);
}

void dtof_loop() {

  dToF.startMeasuring();

  //   if (dToF.startMeasuring(myResults)) {
  //     // Serial.println("num_results:");
  //     // Serial.println(myResults.num_results);

  //     // Serial.println("Measurement:");
  //     // Serial.print("     Result Number: ");
  //     // Serial.print(myResults.result_num);
  //     // Serial.print("  Number of Results: ");
  //     // Serial.println(myResults.num_results);

  //     // for (int i = 0; i < myResults.num_results; ++i) {
  //     //   // 打印到串口
  //     //   Serial.print("       conf: ");
  //     //   Serial.print(myResults.results[i].confidence);
  //     //   Serial.print(" distance mm: ");
  //     //   Serial.print(myResults.results[i].distance_mm);
  //     //   Serial.print(" channel: ");
  //     //   Serial.print(myResults.results[i].channel);
  //     //   Serial.print(" sub_capture: ");
  //     //   Serial.println(myResults.results[i].sub_capture);
  //     // }

  //     int validIndex = 0; // 用于跟踪有效的区域索引
  //     for (int i = 0; i < myResults.num_results; ++i) {

  //       // 跳过无效区域（区域 9 和 18），但是不影响有效区域的位置
  //       if (myResults.results[i].channel == 9 || myResults.results[i].channel
  //       == 18) {
  //         continue; // 直接跳过区域 9 和 18
  //       }

  //       // 计算有效区域的行列位置
  //       int row = validIndex / GRID_SIZE;
  //       int col = validIndex % GRID_SIZE;
  //       validIndex++; // 有效区域索引递增
  //       if (myResults.results[i].confidence >= CONFIDENCE_THRESHOLD) {
  //         uint16_t color =
  //         getColorForDistance(myResults.results[i].distance_mm);

  //         // 绘制方格
  //         sprite.fillRect(col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE,
  //         CELL_SIZE,
  //                         color);

  //         // 显示距离值
  //         char distanceText[10];
  //         sprintf(distanceText, "%d", myResults.results[i].distance_mm);
  //         int textX = col * CELL_SIZE + CELL_SIZE / 2 -
  //                     sprite.textWidth(distanceText) / 2;  // 居中
  //         int textY = row * CELL_SIZE + CELL_SIZE / 2 - 8; // 字体高度约为 16
  //         sprite.setTextColor(TFT_WHITE, color);
  //         sprite.drawString(distanceText, textX, textY);
  //       }
  //     }

  //     // 将绘制的内容推送到屏幕
  //     sprite.pushSprite(0, 0);
  //   } else {
  //     sprite.fillSprite(TFT_BLACK);
  //     sprite.drawString("Measurement failed!", 10, 30);
  //     sprite.pushSprite(0, 0);
  //   }
}
