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
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <graphics.h>
#include <time.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <string>
#include <gdiplus.h>
#include <chrono>
#include <Windows.h>
#include <fstream>    // 文件输入输出流（用于文件读写）
#include "json.hpp"   // JSON库头文件（需要确保该文件在项目目录中）
#include <stdexcept>
#pragma comment(lib,"gdiplus.lib")
using json = nlohmann::json;
using namespace Gdiplus;
using namespace std;
using namespace sf;

#define WD_width 1500		//window_width,图形窗口的宽度
#define WD_height 950
