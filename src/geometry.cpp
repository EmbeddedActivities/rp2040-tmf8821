#include <math.h>
#include <stdio.h>

// 计算直线与 X 轴的夹角（单位：度）
float calculateAngleWithXAxis(float x1, float y1, float x2, float y2) {
    // 计算斜率
    float slope = (y2 - y1) / (x2 - x1);

    // 计算夹角（弧度）
    float angle_rad = atan(slope);

    // 转换为度
    float angle_deg = angle_rad * 180.0 / M_PI;
    return angle_deg;
}

// 计算直线与 X 轴的垂直距离
float calculatePerpendicularDistance(float x1, float y1, float x2, float y2) {
    // 计算斜率
    float slope = (y2 - y1) / (x2 - x1);

    // 计算截距
    float intercept = y1 - slope * x1;

    // 计算直线与 X 轴的交点
    float x_intercept = -intercept / slope;

    // 计算垂直距离（与原点的距离）
    return fabs(x_intercept);
}
