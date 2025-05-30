#include "head.h"
#include "record.h"
#include "button.h"


const TCHAR* LevelChars[] = {
	_T("壹"), _T("贰"), _T("叁"),
	_T("肆"), _T("伍"), _T("陆"),
	_T("柒"), _T("捌"), _T("玖")
};

//初始化按钮数组（主菜单按钮）
vector<unique_ptr<BaseButton>> initMainMenuBtn() {			//返回智能指针方便指定返回的按钮类型
	vector<unique_ptr<BaseButton>> buttons;

	buttons.emplace_back(make_unique <Button>(0.5f, 0.4f, 180, 60, _T("关卡选择"),
		RGB(100, 200, 100), RGB(150, 250, 150))
	);

	buttons.emplace_back(make_unique <Button>(0.5f, 0.5f, 180, 60, _T("商店"),
		RGB(100, 200, 100), RGB(150, 250, 150))
	);

	buttons.emplace_back(make_unique <Button>(0.5f, 0.6f, 180, 60, _T("背包"),
		RGB(100, 200, 100), RGB(150, 250, 150))
	);

	buttons.emplace_back(make_unique <Button>(0.5f, 0.7f, 180, 60, _T("成就"),
		RGB(100, 200, 100), RGB(150, 250, 150))
	);

	buttons.emplace_back(make_unique <Button>(0.5f, 0.8f, 180, 60, _T("退出游戏"),
		RGB(100, 200, 100), RGB(150, 250, 150))
	);

	return move(buttons);
}

//初始化按钮数组（关卡按钮）
vector<unique_ptr<BaseButton>> initLevelBtn(PlayerData player) {

	vector<unique_ptr<BaseButton>> buttons;

	int btnsize = 60;
	double paceX = 0.15f;		//两个按钮之间的距离（x）
	double paceY = 0.2f;		//两个按钮之间的距离（y）
	double startX = 0.1f;		//初始按钮位置（x）
	double startY = 0.15f;

	for (int i = 0;i < 9;i++) {

		COLORREF color, press_color;
		if (i + 1 <= player.unlockLevel) {		//根据关卡的解锁状态来使用不同的颜色
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

		buttons.emplace_back(make_unique <Button>(
			startX + paceX * col, startY + paceY * row,
			btnsize, btnsize,
			LevelChars[i], color, press_color)
		);
	}

	//返回按钮
	buttons.emplace_back(make_unique <Button>(0.8f, 0.2f, 180, 90, _T("返回"),
		RGB(150, 150, 150), RGB(200, 200, 200))
	);

	return move(buttons);
}


//初始化按钮数组（商店按钮）
vector<unique_ptr<BaseButton>> initShopBtn(PlayerData player, vector<ShopItemConfig> shopConfigs, string path) {
	cout << "正在初始化商店按钮" << endl;
	vector<unique_ptr<BaseButton>> buttons;

	int btnW = 110;
	int btnH = 60;
	double paceX = 0.18f;
	//double paceY = 0.2f;
	double startX = 0.2f;
	double startY = 0.7f;

	int imgbtnW = 200;
	int imgbtnH = 420;
	double imgpaceX = 0.18f;
	//double paceY = 0.2f;
	double imgstartX = 0.2f;
	double imgstartY = 0.4f;

	for (int i = 0;i < shopConfigs.size();i++) {		//遍历所有的商品
		cout << i << endl;
		COLORREF color, press_color;
		if (shopConfigs[i].unique == false || (shopConfigs[i].unique && player.items[i].number == 0)) {		//根据物品是否可以购买来使用不同的颜色
			//可购买物品：橙色，浅橙
			color = RGB(220, 130, 30);
			press_color = RGB(240, 150, 50);

		}
		else {
			//不可购买物品：灰色，浅灰
			color = RGB(150, 150, 150);
			press_color = RGB(180, 180, 180);
		}

		int col = i % 3;

		buttons.emplace_back(make_unique <Button>(
			startX + paceX * col, startY,
			btnW, btnH,
			_T("购买"), color, press_color)
		);
	}

	for (int i = 0;i < shopConfigs.size();i++) {		//根据商品数遍历需要的图片

		string imagepath;
		imagepath = path + to_string(i + 1) + ".png";
		int row = i / 3;
		int col = i % 3;

		buttons.emplace_back(make_unique <ImageButton>(
			imgstartX + imgpaceX * col, imgstartY,
			imgbtnW, imgbtnH, imagepath)
		);
	}

	//返回按钮
	buttons.emplace_back(make_unique <Button>(0.8f, 0.2f, 180, 90, _T("返回"),
		RGB(150, 150, 150), RGB(200, 200, 200))
	);
	cout << "初始化商店按钮完毕" << endl;
	return move(buttons);
}

//初始化按钮数组（背包按钮）
vector<unique_ptr<BaseButton>> initBagBtn(PlayerData player, vector <ShopItemConfig> shopConfigs, string path) {

	vector<unique_ptr<BaseButton>> buttons;

	int btnsize = 60;
	//double paceX = 0.15f;
	double paceY = 0.12f;
	double startX = 0.15f;
	double startY = 0.2f;

	for (int i = 0;i < player.items.size();i++) {		//遍历所有玩家可持有的物品

		string imagepath;
		imagepath = path + to_string(i + 1) + ".png";

		if (player.items[i].number <= 0) {				//若没有此物品则使用伪按钮
			buttons.emplace_back(make_unique <FakeButton>(
				startX, startY + paceY * i,
				btnsize, btnsize, imagepath, true)
			);
		}
		else {
			buttons.emplace_back(make_unique <ImageButton>(
				startX, startY + paceY * i,
				btnsize, btnsize, imagepath)
			);
		}
	}

	//返回按钮
	buttons.emplace_back(make_unique <Button>(0.8f, 0.2f, 180, 90, _T("返回"),
		RGB(150, 150, 150), RGB(200, 200, 200))
	);

	return move(buttons);
}


//初始化按钮数组（成就系统按钮）
vector<unique_ptr<BaseButton>> initAchieveBtn(PlayerData player, vector<AchievementConfig> achConfigs, string path) {
	cout << "正在初始化成就界面按钮" << endl;

	//成就系统和商店系统目前都想做成一样的，可以加上翻页的设定
	vector<unique_ptr<BaseButton>> buttons;

	int imgbtnW = 180;
	int imgbtnH = 380;
	double imgpaceX = 0.22f;
	//double paceY = 0.2f;
	double imgstartX = 0.1f;
	double imgstartY = 0.4f;

	for (int i = 0;i < achConfigs.size();i++) {		//遍历所有成就

		string imagepath;
		if (!player.achs[i].unlock) {				//若没有解锁此成就则指定imagepath为问号图片
			imagepath = path + to_string(0) + ".png";
		}
		else {
			imagepath = path + to_string(i + 1) + ".png";
		}

		buttons.emplace_back(make_unique <ImageButton>(
			imgstartX + imgpaceX * i, imgstartY,
			imgbtnW, imgbtnH, imagepath)
		);

	}

	//返回按钮
	buttons.emplace_back(make_unique <Button>(0.85f, 0.1f, 140, 70, _T("返回"),
		RGB(150, 150, 150), RGB(200, 200, 200))
	);
	cout << "初始化成就按钮完毕" << endl;
	return move(buttons);
}

