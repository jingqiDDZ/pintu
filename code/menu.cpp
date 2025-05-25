#include <graphics.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

#define WD_width 1000		//window_width,图形窗口的宽度
#define WD_height 800


const TCHAR* LevelChars[] = {
	_T("壹"), _T("贰"), _T("叁"),
	_T("肆"), _T("伍"), _T("陆"),
	_T("柒"), _T("捌"), _T("玖")
};

//按钮类
class Button {
public:
	int x, y, width, height;		//大小位置
	const TCHAR* text;				//按钮上的文字
	COLORREF color, press_color;			//按钮颜色，鼠标指针悬停在上方后颜色
	bool is_pressed;				//判断指针是否位于按钮上方

	//构造函数
	Button() {

	}

	Button(double x_pos, double y_pos, int w, int h, const TCHAR* t, COLORREF c, COLORREF pc) :
		width(w), height(h), text(t), color(c), press_color(pc) {
		is_pressed = false;

		//传入的x_pos为——目标按钮位于窗口左侧的相对位置与窗口宽度的比值，比如居中则传入1/2，右侧1/3则传入2/3
		int	tx = (WD_width)*x_pos - width / 2;		//将相对位置的比例转换为实际的x位置
		int	ty = (WD_height)*y_pos - height / 2;		//将相对位置的比例转换为实际的x位置
		x = tx;	y = ty;
	}

	//绘制按钮的函数
	void draw() {

		//绘制长方形的按钮
		setfillcolor(is_pressed ? press_color : color);
		fillrectangle(x, y, x + width, y + height);

		//绘制边框
		setfillcolor(BLACK);
		rectangle(x, y, x + width, y + height);

		//绘制文字
		setbkmode(TRANSPARENT);		//文字背景透明
		settextcolor(BLACK);
		int font_size = height * 0.6;			//根据按钮高度设置文字高度
		settextstyle(font_size, 0, _T("宋体"));
		int textW = textwidth(text);
		int times = 0;			//最多循环次数
		//循环缩小字体直到文字宽度适配按钮()
		while (textW > width && times < 5) {
			times++;
			font_size *= 0.8;
			settextstyle(font_size, 0, _T("宋体"));
			textW = textwidth(text);
		}
		int textH = textheight(text);
		//居中绘制
		outtextxy(x + (width - textW) / 2, y + (height - textH) / 2, text);
	}

	//检查鼠标是否位于按钮上方（悬停或者点击都用这个）
	bool checkAbove(int mouseX, int mouseY) {
		if (mouseX >= x && mouseY >= y && mouseX <= x + width && mouseY <= y + height)
			is_pressed = true;
		else
			is_pressed = false;
		return is_pressed;
	}


};

//游戏数据（目前只是一个草稿）
class GameData {
public:
	int unlockLevel = 1;		//解锁的关卡进度
	int coins = 9999;			//钱币数
};

GameData game_data;

//菜单状态枚举（没有添加进入关卡的操作）
enum MenuState {
	MAIN_MENU,
	LEVEL_SELECT,
	SHOP,
	EXIT
};


//初始化按钮数组（主菜单按钮）
vector<Button> initMainMenuBtn() {
	vector<Button> buttons;

	buttons.emplace_back(0.5f, 0.4f, 180, 100, _T("关卡选择"),
		RGB(100, 200, 100), RGB(150, 250, 150));

	buttons.emplace_back(0.5f, 0.6f, 180, 100, _T("商店"),
		RGB(100, 200, 100), RGB(150, 250, 150));

	buttons.emplace_back(0.5f, 0.8f, 180, 100, _T("退出游戏"),
		RGB(100, 200, 100), RGB(150, 250, 150));

	return buttons;
}

//初始化按钮数组（关卡按钮）
vector<Button> initLevelBtn() {

	vector<Button> buttons;

	int btnsize = 60;
	double paceX = 0.15f;
	double paceY = 0.2f;
	double startX = 0.2f;
	double startY = 0.3f;

	for (int i = 0;i < 9;i++) {

		COLORREF color, press_color;
		if (i + 1 <= game_data.unlockLevel) {		//根据关卡的解锁状态来使用不同的颜色
			//已解锁关卡：橙色，浅橙
			color = RGB(220, 130, 30);
			press_color = RGB(240, 150, 50);

		}
		else {
			//未解锁关卡：灰色，浅灰
			color = RGB(150, 150, 150);
			press_color = RGB(180, 180, 180);
		}

		int row = i / 3;
		int col = i % 3;

		buttons.emplace_back(
			startX + paceX * col, startY + paceY * row,
			btnsize, btnsize,
			LevelChars[i], color, press_color
		);
	}

	//返回按钮
	buttons.emplace_back(0.35f, 0.85f, 180, 90, _T("返回"),
		RGB(150, 150, 150), RGB(200, 200, 200));

	return buttons;
}


void drawMainMenu(vector<Button> buttons) {
	cleardevice();

	//绘制背景
	setbkcolor(RGB(240, 240, 240));
	cleardevice();

	//绘制按钮
	for (auto btn : buttons) {
		btn.draw();
	}

	//显示金币数量
	settextstyle(20, 0, _T("宋体"));
	TCHAR coinText[50];
	_stprintf_s(coinText, _T("金币: %d"), game_data.coins);
	outtextxy(850, 30, coinText);
}

void drawLevelSelect(vector<Button> buttons) {
	cleardevice();

	//绘制背景
	setbkcolor(RGB(240, 240, 240));
	cleardevice();

	//绘制按钮
	for (auto btn : buttons) {
		btn.draw();
	}

	//显示金币数量
	settextstyle(20, 0, _T("宋体"));
	TCHAR coinText[50];
	_stprintf_s(coinText, _T("金币: %d"), game_data.coins);
	outtextxy(850, 30, coinText);
}

int menu() {
	// 初始化窗口
	initgraph(WD_width, WD_height);
	SetWindowText(GetHWnd(), _T("游戏主菜单"));
	setbkcolor(WHITE);
	cleardevice();
	BeginBatchDraw();			//先在内存里面绘图

	// 初始化菜单状态
	MenuState currentState = MAIN_MENU;
	vector<Button> buttons = initMainMenuBtn();

	//主循环
	while (true) {
		cleardevice();

		//检查鼠标位置
		MOUSEMSG msg;
		if (MouseHit()) {
			msg = GetMouseMsg();

			//更新按钮悬停状态
			for (auto& btn : buttons) {
				btn.checkAbove(msg.x, msg.y);
			}

			//处理鼠标点击
			if (msg.uMsg == WM_LBUTTONDOWN) {
				for (size_t i = 0; i < buttons.size(); i++) {
					if (buttons[i].checkAbove(msg.x, msg.y)) {

						//主菜单按钮处理
						if (currentState == MAIN_MENU) {
							if (i == 0) {		//关卡选择
								currentState = LEVEL_SELECT;
								buttons = initLevelBtn();
							}
							else if (i == 1) {		//商店
								currentState = SHOP;
								//buttons = initLevelBtn();
							}
							else if (i == 2) {		//退出游戏
								currentState = EXIT;
								//buttons = initLevelBtn();
							}
						}

						//关卡选择界面按钮处理
						else if (currentState == LEVEL_SELECT) {
							if (i < 9) {		//点击关卡按钮
								int level = i + 1;
								if (level > game_data.unlockLevel) {
									cout << "关卡未解锁" << endl;
								}
								else {
									//此处添加关卡代码
									cout << "开始关卡" << level << endl;
								}
							}
							else if (i == 9) {		//返回按钮
								currentState = MAIN_MENU;
								buttons = initMainMenuBtn();
							}
						}

						break;			//一次处理一个按钮点击操作，处理完毕即可退出循环
					}
				}
			}
		}
		// 检查退出
		if (currentState == EXIT) {
			break;
		}

		// 绘制当前菜单
		switch (currentState) {
		case MAIN_MENU:
			drawMainMenu(buttons);
			break;
		case LEVEL_SELECT:
			drawLevelSelect(buttons);
			break;
		case SHOP:
			;
		case EXIT:
			break;
		}
		FlushBatchDraw();
		Sleep(1);  // 降低CPU占用
	}

	closegraph();
	return 0;
}