#include "head.h"
#include "record.h"
#include "button.h"


//加载静态配置数据(游戏启动时调用一次)
void loadConfig(vector<ShopItemConfig>& shopConfigs, vector<AchievementConfig>& achConfigs) {
    ifstream file("config.json");
    if (!file.is_open()) {
        cout << "无法打开 config.json 文件！" << endl;
        exit(1);
    }

    json configJson = json::parse(file);      //解析json格式数据

    //读取商店配置
    for (auto it : configJson["shopItems"]) {
        ShopItemConfig itemNow;
        itemNow.id = it["id"];
        itemNow.text = it["text"];
        itemNow.price = it["price"];
        itemNow.unique = it["unique"];
        shopConfigs.push_back(itemNow);
    }

    //读取成就配置
    for (auto it : configJson["achievements"]) {
        AchievementConfig achNow;
        achNow.id = it["id"];
        achNow.text = it["text"];
        if (it.contains("reward")) {
            achNow.reward = it["reward"];
        }
        achConfigs.push_back(achNow);
    }

}


//加载存档数据
void loadSave(PlayerData& player) {
    ifstream file("save.json");
    if (!file.is_open()) {
        cout << "无存档，使用默认初始数据" << endl;
        player.coins = 500;
        player.unlockLevel = 1;
        return;
    }

    json js = json::parse(file);      //解析json格式数据

    player.coins = js["player"]["coins"];       //开始从js读取相应的数据存放与player内（自动识别变量类型）
    player.unlockLevel = js["player"]["unlockedLevel"];
    player.skillQ = js["player"]["skillQ"];
    player.skillE = js["player"]["skillE"];

    for (auto it : js["itemStatus"]) {
        ItemStatus Now;
        Now.id = it["id"];
        Now.number = it["number"];
        player.items.push_back(Now);
    }

    for (auto it : js["achievementStatus"]) {
        AchievementStatus Now;
        Now.id = it["id"];
        Now.unlock = it["unlock"];
        player.achs.push_back(Now);
    }

    cout << "存档加载成功！当前金币：" << player.coins << endl;

}


//保存游戏数据
void saveData(PlayerData player) {      //这里不传引用
    json j;

    //保存玩家数据
    j["player"]["coins"] = player.coins;
    j["player"]["unlockedLevel"] = player.unlockLevel;
    j["player"]["skillQ"] = player.skillQ;
    j["player"]["skillE"] = player.skillE;

    //商店（背包物品）数据
    for (auto it : player.items) {
        json jNow;
        jNow["id"] = it.id;
        jNow["number"] = it.number;
        j["itemStatus"].push_back(jNow);
    }

    //成就数据
    for (auto it : player.achs) {
        json jNow;
        jNow["id"] = it.id;
        jNow["unlock"] = it.unlock;
        j["achievementStatus"].push_back(jNow);
    }

    //写入文件
    ofstream file("save.json");
    if (file.is_open()) {
        file << j.dump(4);
        cout << "游戏进度已保存！" << endl;
    }
    else {
        cout << "错误：无法保存存档" << endl;
    }
}


/*
//原本在这里定义的全局数据
PlayerData player; // 玩家动态数据
vector<ShopItemConfig> shopConfigs; // 商店配置（只读）
vector<AchievementConfig> achConfigs; // 成就配置（只读）
*/
