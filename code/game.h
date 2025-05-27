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
#pragma comment(lib,"gdiplus.lib")
using namespace Gdiplus;
using namespace std;


void loadSoundBgm(const string path, sf::Sound& sound, sf::SoundBuffer& buffer);			//加载声音文件的函数(BGM,)
void loadSoundClip(const string path, sf::Sound& sound, sf::SoundBuffer& buffer);	//加载声音文件的函数

