#include "dialogue.h"
#include <Windows.h> // 需要包含 Windows 头文件（utf_8转换）

	
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

	// 加载对话内容
	loadText(path);
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
		members.emplace_back(mnow);
	}
}

//ai代码，防止outtextxy输出json读取的中文显示乱码
std::wstring utf8_to_wstring(const std::string& str) {
	if (str.empty()) return L"";

	// 计算需要的宽字符缓冲区大小
	int size_needed = MultiByteToWideChar(
		CP_UTF8, 0,
		str.c_str(), (int)str.size(),
		NULL, 0
	);

	if (size_needed <= 0) {
		return L"转换失败";
	}

	// 分配缓冲区
	std::wstring result(size_needed, 0);

	// 执行转换
	MultiByteToWideChar(
		CP_UTF8, 0,
		str.c_str(), (int)str.size(),
		&result[0], size_needed
	);

	return result;
}

// 加载后预处理alpha通道
void preprocessAlpha(IMAGE* pImg) {
	DWORD* pBuf = GetImageBuffer(pImg);
	int size = pImg->getwidth() * pImg->getheight();

	for (int i = 0; i < size; i++) {
		BYTE alpha = (pBuf[i] >> 24) & 0xFF;
		if (alpha == 0) {
			pBuf[i] = 0x00000000; // 确保完全透明像素为0
		}
	}
}

//绘制对话（整个数组都要）
bool Dialogue::draw() {
	cout << "开始绘制对话" << endl;
	// 计算对话框位置(居中偏下)


	for (auto it : members) {
		cleardevice(); // 清屏，防止内容叠加

		//加载人物图片
		IMAGE charaImg;
		wstring wpath(it.imgpath.begin(), it.imgpath.end());
		loadimage(&charaImg, wpath.c_str(), charaW, charaH, true);
		preprocessAlpha(&charaImg);
		//绘制说话人物
		putimage(charaX, charaY, &charaImg, SRCAND);
		//putimage(charaX, charaY, &charaImg, SRCPAINT);
		//putimage_alpha(x, y, &img);

		//绘制通用的对话框和姓名框
		putimage(boxX, boxY, &textBox);
		putimage(nameX, nameY, &nameBox);


		

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
		settextstyle((int)nameH * 0.7, 0, _T("宋体"));
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
