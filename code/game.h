#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <graphics.h>
#include <time.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <string>
using namespace std;


sf::Sound loadSoundBgm(const string path);			//加载声音文件的函数(BGM,)
sf::Sound loadSoundClip(const string path);		//加载声音文件的函数

//int playSound(const string sound_name);			//播放声音文件的函数
