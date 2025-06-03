#pragma once
#include "head.h"
#include "button.h"

struct DiaMember {
	string imgpath;			//人物头像路径
	string bkpath;			//背景图路径
	string name;			//说话人名称
	string text;			//对话内容文本
};

class Dialogue {
public:
	vector<DiaMember> members;		//对话列表
	int currentIndex = 0;			//当前对话索引
	ImageButton nextButton;			//继续按钮(想用右箭头)
	IMAGE nameBox;					//名字框图像
	IMAGE textBox;					//对话框图像

	int boxW, boxH;
	int boxX, boxY;
	int nameW, nameH;
	int nameX, nameY;
	int charaW = WD_height * 0.6 * 0.55, charaH = WD_height * 0.6;
	int charaX = WD_width - WD_height * 0.6 * 0.55 - 10, charaY = WD_height * 0.3;


	Dialogue(string);
	bool draw();		//绘制对话（整个数组都要）

private:
	void loadText(string);		//从json文件加载数据

};