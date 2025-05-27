#include <iostream>   // 输入输出流（用于cout/cin等）
#include <fstream>    // 文件输入输出流（用于文件读写）
#include <vector>     // 动态数组容器（用于存储已购物品列表）
#include "json.hpp"   // JSON库头文件（需要确保该文件在项目目录中）

// 使用nlohmann库的json类型别名（简化代码）
using json = nlohmann::json;
using namespace std;

//所有函数的声明
void loadSave();
void loadConfig();
void saveData();


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

//全局数据 
PlayerData player; // 玩家动态数据
vector<ShopItemConfig> shopConfigs; // 商店配置（只读）
vector<AchievementConfig> achConfigs; // 成就配置（只读）

//加载静态配置数据(游戏启动时调用一次)
void loadConfig() {
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
void loadSave() {
    ifstream file("save.json");
    if (!file.is_open()) {
        cout << "无存档，使用默认初始数据" << endl;
        player.coins = 500;
        player.unlockLevel = 1;
        return;
    }

    json js = json::parse(file);      //解析json格式数据

    player.coins = js["player"]["coins"];
    player.unlockLevel = js["player"]["unlockedLevel"];

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
void saveData() {
    json j;

    //保存玩家数据
    j["player"]["coins"] = player.coins;
    j["player"]["unlockedLevel"] = player.unlockLevel;

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

