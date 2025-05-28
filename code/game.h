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
#pragma comment(lib,"gdiplus.lib")
using namespace Gdiplus;
using namespace std;


void loadSoundBgm(const string path, sf::Sound& sound, sf::SoundBuffer& buffer);			//加载声音文件的函数(BGM,)
void loadSoundClip(const string path, sf::Sound& sound, sf::SoundBuffer& buffer);	//加载声音文件的函数

