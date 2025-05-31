// PathFunctions.h
#pragma once
#include <cmath>
#include "code/game.h"

// ����·����Ĭ�ϣ�
void linearPath(float progress, int startX, int startY, int endX, int endY, int& x, int& y) {
    x = startX + static_cast<int>((endX - startX) * progress);
    y = startY + static_cast<int>((endY - startY) * progress);
}

// ������·��
void parabolicPath(float progress, int startX, int startY, int endX, int endY, int& x, int& y) {
    x = startX + static_cast<int>((endX - startX) * progress);
    // �����߷��� y = a*x^2������򻯼���
    float height = 100.0f; // �����߸߶�
    y = startY + static_cast<int>((endY - startY) * progress - height * (progress - 0.5f) * (progress - 0.5f) * 4);
}

// Բ��·��
void circularPath(float progress, int centerX, int centerY, int radius, int& x, int& y) {
    float angle = progress * 2 * 3.14159f; // ����Բ��
    x = centerX + static_cast<int>(cos(angle) * radius);
    y = centerY + static_cast<int>(sin(angle) * radius);
}

// ����Ч��·��
void heartbeatPath(float progress, int centerX, int centerY, int& x, int& y) {
    x = centerX;
    // �������Σ���������
    float pulse = 0;
    if (progress < 0.3f) {
        pulse = sin(progress * 10 * 3.14159f) * 20;
    }
    else if (progress < 0.7f) {
        pulse = sin((progress - 0.3f) * 7 * 3.14159f) * 15;
    }
    y = centerY + static_cast<int>(pulse);
}

//�����ϵ����µĶԽ����ƶ�
void diagonalPath(float progress, int& x, int& y) {
    int screenWidth = getwidth();
    int screenHeight = getheight();
    x = static_cast<int>(screenWidth * progress - debuffAnimation.width);
    y = static_cast<int>(screenHeight * progress - debuffAnimation.height);
}

// ·������ʾ��������Ļ���������
void flyInPath(float progress, int& x, int& y) {
    int screenWidth = getwidth();
    int screenHeight = getheight();

    if (progress < 0.5f) {
        // ǰ��Σ���������
        x = static_cast<int>(-debuffAnimation.width + (screenWidth / 2 + debuffAnimation.width / 2) * progress * 2);
        y = screenHeight / 2 - debuffAnimation.height / 2;
    }
    else {
        // ���Σ���΢���¸���
        x = screenWidth / 2 - debuffAnimation.width / 2;
        float offset = sin((progress - 0.5f) * 2 * 3.14159f) * 20;
        y = screenHeight / 2 - debuffAnimation.height / 2 + static_cast<int>(offset);
    }
}