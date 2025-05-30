/***************************************************************
 * 文件功能: 菜单状态种类声明，绘制菜单函数声明
 *
 * 主要包含:
 *   - 菜单状态枚举
 *
 * 修改记录:
 *   2025-05-30 v1.0
 ***************************************************************/


#pragma once 
#include "head.h"
#include "button.h"
#include "record.h"

 //菜单状态枚举（没有添加进入关卡的操作）
enum MenuState {
	MAIN_MENU,
	LEVEL_SELECT,
	SHOP,
	BAG,
	ACHIEVE,
	EXIT
};


void drawMainMenu(vector<unique_ptr<BaseButton>>& buttons, PlayerData player);
void drawLevelSelect(vector<unique_ptr<BaseButton>>& buttons, PlayerData player);
void drawShop(vector<unique_ptr<BaseButton>>& buttons, PlayerData player);
void drawBag(vector<unique_ptr<BaseButton>>& buttons, PlayerData player, vector<ShopItemConfig> shopConfigs);
void drawAchieve(vector<unique_ptr<BaseButton>>& buttons, PlayerData player, vector <AchievementConfig> achConfigs);

