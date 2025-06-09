/***************************************************************
 * 文件功能: 游戏主头文件，包含所有必要的库引用、宏定义和全局声明
 *
 * 主要包含:
 *   - 各种库文件的引用(SFML、EasyX、JSON等)
 *   - 窗口大小宏定义
 *   - Gdiplus相关设置
 *
 * 修改记录:
 *   2025-05-30 v1.0
 ***************************************************************/

#pragma once	//防止重复包含，可以解决许多重复定义的报错
#include <SFML/Graphics.hpp>   // SFML图形库，用于处理音视频、图像等多媒体功能
#include <SFML/Audio.hpp>      // SFML音频库，用于播放音效和背景音乐
#include <graphics.h>          // EasyX图形库，主要用于2D图形界面绘制
#include <time.h>              // C标准库，提供时间和日期相关函数
#include <stdio.h>             // C标准库，提供输入输出函数（如printf、scanf等）
#include <conio.h>             // 控制台输入输出，常用于检测键盘输入
#include <iostream>            // C++标准库，提供输入输出流（如cin、cout等）
#include <vector>              // C++标准库，提供动态数组容器
#include <string>              // C++标准库，提供字符串处理功能
#include <gdiplus.h>           // GDI+库，用于高级图形处理（如图片、透明度等）
#include <chrono>              // C++11标准库，提供高精度计时功能
#include <Windows.h>           // Windows API，提供窗口、消息、系统相关操作
#include <fstream>             // C++标准库，文件输入输出流（用于文件读写）
#include "json.hpp"            // JSON库头文件（nlohmann/json，用于解析和生成JSON数据）
#include <stdexcept>           // C++标准库，提供标准异常类
#pragma comment(lib,"gdiplus.lib")
using json = nlohmann::json;
using namespace Gdiplus;
using namespace std;
using namespace sf;

#define WD_width 1500		//window_width,图形窗口的宽度
#define WD_height 950
