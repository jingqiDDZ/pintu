/***************************************************************
 * �ļ�����: �˵�״̬�������������Ʋ˵���������
 *
 * ��Ҫ����:
 *   - �˵�״̬ö��
 *
 * �޸ļ�¼:
 *   2025-05-30 v1.0
 ***************************************************************/


#pragma once 
#include "head.h"
#include "button.h"
#include "record.h"

 //�˵�״̬ö�٣�û����ӽ���ؿ��Ĳ�����
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

