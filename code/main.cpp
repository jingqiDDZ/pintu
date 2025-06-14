﻿#include "head.h"
#include "record.h"
#include "button.h"
#include "menu.h"
#include "music.h"
#include "level.h"
#include "dialogue.h"
#include "pause.h"


//全局数据 
vector<unique_ptr<Level>> levelgames;         // 关卡对象列表，存储所有关卡实例
ConfirmWindow confirm_window(_T("是否确认"));  // 全局确认弹窗（如购买、进入关卡时使用）
ImageWindow image_window;                     // 全局图片弹窗（如成就CG、物品介绍等）
PlayerData player;                            // 玩家动态数据（金币、解锁关卡、技能等）
vector<ShopItemConfig> shopConfigs;           // 商店配置（只读，包含所有可购买物品的信息）
vector<AchievementConfig> achConfigs;         // 成就配置（只读，包含所有成就的信息）
Sound main_bgm;                              // 主菜单背景音乐
SoundBuffer main_buffer;                     // 主菜单背景音乐缓冲区
bool bgm_play = false;                       // 主菜单BGM是否正在播放
int Level_5::value_5 = Level_5::Prob;        // Level_5关卡的概率参数初始化
//Level_TE levelgame(0, 3, 1);	//关卡
//Level levelnormal(2, 3, 1);		//一般关卡测试
//Dialogue testDia("./assets/text/level/0/dialogue.json");

int main() {
	//加载游戏数据
	loadConfig(shopConfigs, achConfigs);
	loadSave(player);
	cout << player.items.size() << endl;
	cout << shopConfigs.size() << endl;
	cout << "存档处理完毕，展示用户持有物品id" << endl;
	bgm_play = false;
	loadSoundBgm("./assets/audio/main/main_pure.wav", main_bgm, main_buffer);

	for (int i = 0;i < player.items.size();i++) {
		cout << player.items[i].id << endl;
	}

	//初始化关卡列表
	levelgames.push_back(make_unique<Level>(0, 3, 1,player.skillQ,player.skillE));
	levelgames.push_back(make_unique<Level>(1, 3, 1, player.skillQ, player.skillE));
	levelgames.push_back(make_unique<Level>(2, 3, 0, player.skillQ, player.skillE));
	levelgames.push_back(make_unique<Level_3>(3, 3, 1, player.skillQ, player.skillE));
	levelgames.push_back(make_unique<Level_4>(4, 3, 0, player.skillQ, player.skillE));
	levelgames.push_back(make_unique<Level_5>(5, 4, 1, player.skillQ, player.skillE));
	levelgames.push_back(make_unique<Level_6>(6, 4, 1, player.skillQ, player.skillE));
	levelgames.push_back(make_unique<Level_7>(7, 5, 1, player.skillQ, player.skillE));
	levelgames.push_back(make_unique<Level>(8, 4, 0, player.skillQ, player.skillE));

	// 初始化窗口
	initgraph(WD_width, WD_height);
	SetWindowText(GetHWnd(), _T("游戏主菜单"));
	setbkcolor(WHITE);
	cleardevice();
	BeginBatchDraw();			//先在内存里面绘图

	
	
	FlushBatchDraw();
	//测试对话
	//testDia.draw();

	// 初始化菜单状态
	MenuState currentState = MAIN_MENU;
	vector<unique_ptr<BaseButton>> buttons = initMainMenuBtn();
	string imgpath;				

	//主循环
	while (true) {
		cleardevice();

		if (!bgm_play) {
			main_bgm.play();
			bgm_play = true;
		}
		//检查鼠标位置
		MOUSEMSG msg;
		if (MouseHit()) {
			msg = GetMouseMsg();

			//更新按钮悬停状态
			for (auto& btn : buttons) {
				btn->checkAbove(msg.x, msg.y);
			}

			//处理鼠标点击
			if (msg.uMsg == WM_LBUTTONDOWN) {
				for (size_t i = 0; i < buttons.size(); i++) {
					if (buttons[i]->checkAbove(msg.x, msg.y)) {

						//主菜单按钮处理
						if (currentState == MAIN_MENU) {
							if (i == 0) {		//关卡选择
								currentState = LEVEL_SELECT;
								buttons = initLevelBtn(player);
							}
							else if (i == 1) {		//商店
								cout << "进入商店" << endl;
								currentState = SHOP;
								imgpath = "./assets/image/shop/";
								buttons = initShopBtn(player, shopConfigs, imgpath);
							}
							else if (i == 2) {		//背包
								cout << "打开背包" << endl;
								currentState = BAG;
								imgpath = "./assets/image/bag/";
								buttons = initBagBtn(player, shopConfigs, imgpath);
							}
							else if (i == 3) {		//成就
								cout << "打开成就" << endl;
								currentState = ACHIEVE;
								imgpath = "./assets/image/achievement/";
								buttons = initAchieveBtn(player, achConfigs, imgpath);
							}
							else if (i == 4) {		//退出游戏
								currentState = EXIT;
								//buttons = initLevelBtn();
							}
						}

						//关卡选择界面按钮处理
						else if (currentState == LEVEL_SELECT) {
							if (i < 9) {		//点击关卡按钮
								int level = i + 1;
								if (level > player.unlockLevel) {
									cout << "关卡未解锁" << endl;
								}
								else {
									//暂停画面，并弹出确认弹窗
									//confirm_window.show();
									bool confirmed = false;		//确认是否进行关卡
									drawLevelSelect(buttons, player);
									confirm_window = ConfirmWindow(_T("进入梦境"));
									confirm_window.draw();
									FlushBatchDraw();
									while (true) {
										msg = GetMouseMsg();
										if (msg.uMsg == WM_LBUTTONDOWN) {
											if (confirm_window.confirmbtn.checkAbove(msg.x, msg.y)) {
												confirmed = true;
												break;
											}
											else if (confirm_window.backbtn.checkAbove(msg.x, msg.y)) {
												confirmed = false;
												break;
											}
										}
									}
									main_bgm.pause();	//终止main_bgm

									if (confirmed) {
										//此处添加关卡代码
										//main_bgm.stop();
										bgm_play = false;

										cout << "开始关卡" << level << endl;
										LevelResult result = levelgames[i]->play();
										levelgames[i]->sound_bgm.stop();		//停止播放关卡音频
										levelgames[i]->sound_win.stop();

										//处理返回值，如果win则开始结算目前的关卡进度，金币数，道具
										if (result == LevelResult::Win) {
											player.coins += 100 + 1 * 10;
											if (player.unlockLevel == i + 1 && player.unlockLevel != 9) {
												player.unlockLevel++;
											}
											saveData(player);
										}
									}

									//返回关卡选择界面
									currentState = LEVEL_SELECT;
									buttons = initLevelBtn(player);
									main_bgm.play();	//播放main_bgm
									bgm_play = true;
								}
							}
							else if (i == 9) {		//返回按钮
								currentState = MAIN_MENU;
								buttons = initMainMenuBtn();
							}
						}

						//商店界面按钮处理
						else if (currentState == SHOP) {
							cout << "成功进入商店" << endl;

							if (i < shopConfigs.size() && (shopConfigs[i].unique == false || (shopConfigs[i].unique && player.items[i].number == 0))) {		//点击购买按钮且可以购买
								bool confirmed = false;		//确认是否进行购买
								drawShop(buttons, player);
								confirm_window = ConfirmWindow(_T("是否确认购买"));
								confirm_window.draw();
								FlushBatchDraw();
								while (true) {
									msg = GetMouseMsg();
									if (msg.uMsg == WM_LBUTTONDOWN) {
										if (confirm_window.confirmbtn.checkAbove(msg.x, msg.y)) {
											confirmed = true;
											break;
										}
										else if (confirm_window.backbtn.checkAbove(msg.x, msg.y)) {
											confirmed = false;
											break;
										}
									}
								}

								if (confirmed) {
									cout << "正在购买: 编号" << i << " " << shopConfigs[i].id << endl;
									//此处添加购买代码
									if (shopConfigs[i].price <= player.coins) {
										player.items[i].number++;
										player.coins -= shopConfigs[i].price;
										saveData(player);
										cout << "购买完毕，成功保存" << endl;
										cout << "目前金币数: " << player.coins << endl;
									}
									else {
										cout << "资金不足，购买失败" << endl;
									}
								}
							}
							else if (i == 2 * shopConfigs.size()) {		//返回按钮
								currentState = MAIN_MENU;
								buttons = initMainMenuBtn();
							}
						}

						//背包界面按钮处理
						else if (currentState == BAG) {
							if (i < buttons.size() - 1) {		//背包详情
								confirm_window = ConfirmWindow(_T("将此物品设定为技能——"), _T("E"), _T("Q"));

								bool Qed = false;		//确认是否设定为Q技能
								drawBag(buttons, player, shopConfigs);
								confirm_window.draw();
								FlushBatchDraw();
								while (true) {
									msg = GetMouseMsg();
									if (msg.uMsg == WM_LBUTTONDOWN) {
										if (confirm_window.confirmbtn.checkAbove(msg.x, msg.y)) {
											Qed = true;
											break;
										}
										else if (confirm_window.backbtn.checkAbove(msg.x, msg.y)) {
											Qed = false;
											break;
										}
									}
								}

								if (Qed) {
									player.skillQ = i+1;
									if (player.skillE == i + 1) {	//如果该技能被设置为E，则将E改为0
										player.skillE = 0;
									}			
								}
								else if(!Qed){
									player.skillE = i + 1;
									if (player.skillQ == i + 1) {	//如果该技能被设置为E，则将E改为0
										player.skillQ = 0;
									}
								}
								else {
									;
								}
								saveData(player);
							}
							if (i == buttons.size() - 1) {		//返回按钮
								currentState = MAIN_MENU;
								buttons = initMainMenuBtn();
							}
						}

						//成就界面按钮处理
						else if (currentState == ACHIEVE) {
							if (i < buttons.size() - 1 && i<player.unlockLevel-3) {		//成就详情
								imgpath = "./assets/image/achievement/" + to_string(i) + "/";
								image_window = ImageWindow(0.6, 0.6, imgpath);
								drawAchieve(buttons, player, achConfigs);
								image_window.draw();
								FlushBatchDraw();
								while (true) {
									msg = GetMouseMsg();
									if (msg.uMsg == WM_LBUTTONDOWN) {
										if (image_window.backbtn.checkAbove(msg.x, msg.y)) {
											break;
										}
									}
								}
							}
							else if (i == buttons.size() - 1) {		//返回按钮
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
			drawMainMenu(buttons, player);
			break;
		case LEVEL_SELECT:
			drawLevelSelect(buttons, player);
			break;
		case SHOP:
			drawShop(buttons, player);
		case BAG:
			drawBag(buttons, player, shopConfigs);
		case ACHIEVE:
			drawAchieve(buttons, player, achConfigs);
		case EXIT:
			break;
		}
		FlushBatchDraw();
		Sleep(1);  // 降低CPU占用
	}

	closegraph();
	return 0;
}