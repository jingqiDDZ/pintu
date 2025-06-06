﻿#include "dialogue.h"
#include "putimage_pro.h"
#include "ReadJson.h"

	
Dialogue::Dialogue(string path) {			//path为json文件地址
	//初始化各种位置大小数据
	boxW = WD_width * 0.92;
	boxH = WD_height * 0.28;

	boxX = (WD_width - boxW) / 2;
	boxY = WD_height - boxH - 50;

	nameW = boxW * 0.2;
	nameH = boxH * 0.3;

	nameX = boxX;
	nameY = boxY - nameH;

	charaW = WD_height * 0.6 * 0.65;
	charaH = WD_height * 0.6;
	charaX = WD_width - charaW - (WD_width - boxW);
	charaY = boxY - charaH + 15;

	//初始化按钮
	nextButton = ImageButton(0.85, 0.85, 90, 75, "./assets/image/ui/arrow.png");

	//初始化对话框图片
	loadimage(&textBox, _T("./assets/image/ui/text_box.png"), boxW, boxH, true);
	loadimage(&nameBox, _T("./assets/image/ui/name_box.png"), nameW, nameH, true);
	cout << "ui路径初始化完毕" << endl;

	// 加载对话内容
	loadText(path);
	cout << "对话初始化完毕" << endl;
}

//从json文件加载数据
void Dialogue::loadText(string path){
	ifstream file(path);
	if (!file.is_open()) {
		cout << "无法打开对话文件" << endl;
		exit(1);
	}

	json jsonData;
	file >> jsonData;		//解析json格式数据

	for (auto it : jsonData) {
		DiaMember mnow;
		mnow.name = it["name"];
		mnow.text = it["text"];
		mnow.imgpath = it["imgpath"];
		mnow.bkpath = it["bkpath"];
		members.emplace_back(mnow);
	}
}



//绘制对话（整个数组都要）
bool Dialogue::draw() {
	cout << "开始绘制对话" << endl;
	// 计算对话框位置(居中偏下)


	for (auto it : members) {
		cleardevice(); // 清屏，防止内容叠加

		//绘制对话背景
		IMAGE bkImg;
		wstring wpath(it.bkpath.begin(), it.bkpath.end());
		loadimage(&bkImg, wpath.c_str(), WD_width, WD_height, true);
		putimage_alpha(0, 0, &bkImg);

		//加载人物图片
		IMAGE charaImg;
		wstring wpathh(it.imgpath.begin(), it.imgpath.end());
		loadimage(&charaImg, wpathh.c_str(), charaW, charaH, true);
		//绘制说话人物
		putimage_alpha(charaX, charaY, &charaImg);

		//绘制通用的对话框和姓名框
		putimage_alpha(boxX, boxY, &textBox);
		putimage_alpha(nameX, nameY, &nameBox);

		//绘制说话人姓名
		wstring wname = utf8_to_wstring(it.name);
		wstring wtext = utf8_to_wstring(it.text);
		settextcolor(BLACK);
		setbkmode(TRANSPARENT);
		settextstyle((int)nameH * 0.7, 0, _T("宋体"));
		//wstring wname(it.name.begin(), it.name.end());
		outtextxy(nameX+10, nameY+10, wname.c_str());

		//绘制对话内容
		settextcolor(BLACK);
		setbkmode(TRANSPARENT);
		settextstyle((int)nameH * 0.6, 0, _T("宋体"));
		//wstring wtext(it.text.begin(), it.text.end());
		outtextxy(boxX + 50, boxY + 50, wtext.c_str());

		//绘制按钮
		nextButton.draw();

		FlushBatchDraw();
		//展示对话内容直到用户点击继续按钮
		while (true) {
			MOUSEMSG msg;
			if (MouseHit()) {
				msg = GetMouseMsg();
				//nextButton.checkAbove(msg.x, msg.y);
				if (msg.uMsg == WM_LBUTTONDOWN && nextButton.checkAbove(msg.x, msg.y)) {
					break;
				}
			}
		}	
	}
	return true;		//对话播放完毕，返回true

}
