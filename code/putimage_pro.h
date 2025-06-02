#pragma once
#include "head.h"

// 初始化 GDI+（只需在程序入口处调用一次）
void InitGdiplus();
// 透明绘制函数
void putimage_alpha(int x, int y, IMAGE* img);