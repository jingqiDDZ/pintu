// PathFunctions.h
#pragma once
#include <cmath>
#include "code/game.h"

// 线性路径（默认）
void linearPath(float progress, int startX, int startY, int endX, int endY, int& x, int& y) {
    x = startX + static_cast<int>((endX - startX) * progress);
    y = startY + static_cast<int>((endY - startY) * progress);
}

// 抛物线路径
void parabolicPath(float progress, int startX, int startY, int endX, int endY, int& x, int& y) {
    x = startX + static_cast<int>((endX - startX) * progress);
    // 抛物线方程 y = a*x^2，这里简化计算
    float height = 100.0f; // 抛物线高度
    y = startY + static_cast<int>((endY - startY) * progress - height * (progress - 0.5f) * (progress - 0.5f) * 4);
}

// 圆形路径
void circularPath(float progress, int centerX, int centerY, int radius, int& x, int& y) {
    float angle = progress * 2 * 3.14159f; // 完整圆周
    x = centerX + static_cast<int>(cos(angle) * radius);
    y = centerY + static_cast<int>(sin(angle) * radius);
}

// 心跳效果路径
void heartbeatPath(float progress, int centerX, int centerY, int& x, int& y) {
    x = centerX;
    // 心跳波形：两个脉冲
    float pulse = 0;
    if (progress < 0.3f) {
        pulse = sin(progress * 10 * 3.14159f) * 20;
    }
    else if (progress < 0.7f) {
        pulse = sin((progress - 0.3f) * 7 * 3.14159f) * 15;
    }
    y = centerY + static_cast<int>(pulse);
}

//从左上到右下的对角线移动
void diagonalPath(float progress, int& x, int& y) {
    int screenWidth = getwidth();
    int screenHeight = getheight();
    x = static_cast<int>(screenWidth * progress - debuffAnimation.width);
    y = static_cast<int>(screenHeight * progress - debuffAnimation.height);
}

// 路径函数示例：从屏幕外飞入中心
void flyInPath(float progress, int& x, int& y) {
    int screenWidth = getwidth();
    int screenHeight = getheight();

    if (progress < 0.5f) {
        // 前半段：从左侧飞入
        x = static_cast<int>(-debuffAnimation.width + (screenWidth / 2 + debuffAnimation.width / 2) * progress * 2);
        y = screenHeight / 2 - debuffAnimation.height / 2;
    }
    else {
        // 后半段：轻微上下浮动
        x = screenWidth / 2 - debuffAnimation.width / 2;
        float offset = sin((progress - 0.5f) * 2 * 3.14159f) * 20;
        y = screenHeight / 2 - debuffAnimation.height / 2 + static_cast<int>(offset);
    }
}