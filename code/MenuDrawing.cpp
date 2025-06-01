#include "head.h"
#include "record.h"
#include "button.h"


//绘制主菜单界面
void drawMainMenu(vector<unique_ptr<BaseButton>>& buttons, PlayerData player) {
	cleardevice();

	//绘制背景
	setbkcolor(RGB(240, 240, 240));
	cleardevice();

	//绘制按钮
	for (auto& btn : buttons) {
		btn->draw();
	}

	//显示金币数量
	settextstyle(20, 0, _T("宋体"));
	TCHAR coinText[50];
	_stprintf_s(coinText, _T("金币: %d"), player.coins);
	outtextxy(850, 30, coinText);

}


//绘制关卡选择界面
void drawLevelSelect(vector<unique_ptr<BaseButton>>& buttons, PlayerData player) {
	cleardevice();

	//绘制背景
	setbkcolor(RGB(240, 240, 240));
	cleardevice();

	//绘制按钮
	for (auto& btn : buttons) {
		btn->draw();
	}

	//显示金币数量
	settextstyle(20, 0, _T("宋体"));
	TCHAR coinText[50];
	_stprintf_s(coinText, _T("金币: %d"), player.coins);
	outtextxy(850, 30, coinText);
}


//绘制商店界面
void drawShop(vector<unique_ptr<BaseButton>>& buttons, PlayerData player) {
	cleardevice();

	//绘制背景
	setbkcolor(RGB(240, 240, 240));
	cleardevice();

	//绘制按钮
	for (auto& btn : buttons) {
		btn->draw();
	}

	//显示金币数量
	settextstyle(20, 0, _T("宋体"));
	TCHAR coinText[50];
	_stprintf_s(coinText, _T("金币: %d"), player.coins);
	outtextxy(780, 30, coinText);
}

//绘制背包界面
void drawBag(vector<unique_ptr<BaseButton>>& buttons, PlayerData player, vector<ShopItemConfig> shopConfigs) {
	cleardevice();

	//绘制背景
	setbkcolor(RGB(240, 240, 240));
	cleardevice();

	//绘制按钮
	for (auto& btn : buttons) {
		btn->draw();

		//同时在按钮后方显示物品名(出现了问题，按钮似乎没办法告知自己是什么物品)

	}

	//显示金币数量
	settextstyle(20, 0, _T("宋体"));
	TCHAR coinText[50];
	_stprintf_s(coinText, _T("金币: %d"), player.coins);
	outtextxy(780, 30, coinText);
}

//绘制成就界面
void drawAchieve(vector<unique_ptr<BaseButton>>& buttons, PlayerData player, vector<AchievementConfig> shopConfigs) {
	cleardevice();

	//绘制背景
	setbkcolor(RGB(240, 240, 240));
	cleardevice();

	//绘制按钮
	for (auto& btn : buttons) {
		btn->draw();

		//同时在按钮后方显示物品名(出现了问题，按钮似乎没办法告知自己是什么物品)

	}

	//显示金币数量
	settextstyle(20, 0, _T("宋体"));
	TCHAR coinText[50];
	_stprintf_s(coinText, _T("金币: %d"), player.coins);
	outtextxy(780, 30, coinText);
}