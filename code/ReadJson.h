#pragma once
#include "head.h"
#include <Windows.h> // 需要包含 Windows 头文件（utf_8转换）

//ai代码，防止outtextxy输出json读取的中文显示乱码
std::wstring utf8_to_wstring(const std::string& str);