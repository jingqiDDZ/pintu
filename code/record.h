/***************************************************************
 * 文件功能: 游戏数据配置和玩家数据结构声明
 *
 * 主要包含:
 *   - 商店物品配置结构(ShopItemConfig)
 *   - 成就配置结构(AchievementConfig)
 *   - 物品状态结构(ItemStatus)
 *   - 成就状态结构(AchievementStatus)
 *   - 玩家数据结构(PlayerData)
 *   - 数据加载和保存的函数声明
 *
 * 修改记录:
 *   2025-05-30 v1.0
 ***************************************************************/

#pragma once 
#include "head.h"

 //游戏数据结构体声明
struct ShopItemConfig {
	string id;
	string text;
	int price;
	bool unique;
};

struct AchievementConfig {
	string id;
	string text;
	json reward;
};

struct ItemStatus {
	string id;
	int number;
};

struct AchievementStatus {
	string id;
	bool unlock;
};

struct PlayerData {
	int coins;
	int unlockLevel;
	vector <ItemStatus> items;
	vector <AchievementStatus> achs;
};

//所有函数的声明
void loadSave(PlayerData& player);
void saveData(PlayerData player);
void loadConfig(vector<ShopItemConfig>& shopConfigs, vector<AchievementConfig>& achConfigs);
