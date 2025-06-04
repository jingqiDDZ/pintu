#pragma once
#include "head.h"
#include "music.h"
#include "Animation.h"
#include "dialogue.h"


#define BLOCK_SIZE 100
#define MARGIN  50

//关卡play的返回值
enum class LevelResult {
	Win,   // 通关
	Lose,      // 失败
	Exit,        // 主动退出（如按返回键）
};

//存储正记时相关数据的结构体
struct TimeData {
	time_t startTime, endTime;      // 计时模式用的时间变量
};

//存储倒计时相关数据的结构体
struct CountdownData {
	int totalTime = 300;            // 倒计时总时间(秒)
	int remaining;                  // 剩余时间(倒计时模式用)
	chrono::time_point<chrono::system_clock>startTime, currentTime; // 倒计时时间点
	chrono::duration<double> used;  // 已用时间
	bool isTimeout = false;         // 是否超时(倒计时模式)
	bool Played_count = false;      // 倒计时音乐是否已播放

	Sound sound;            // 倒计时警告音效
	SoundBuffer buffer;     // 倒计时音效缓冲区
};

class Level {
public:

	//需要直接初始化的变量
	int id = 0;						// 关卡名，关联到后续的地址
	int SSIZE = 3;                  // 游戏板尺寸 (3x3, 4x4等) 
	int Tmode = 1;                  // 0=计时模式, 1=倒计时模式

	bool isWinning = false;         // 胜利标志
	vector<vector<int>> board;      // 游戏板状态
	vector<vector<vector<int>>> history; // 操作历史记录(用于回退功能)
	int emptyRow, emptyCol;         // 空白块位置
	int moves = 0;                  // 移动步数计数器

	CountdownData countdownData;	//存储倒计时相关数据
	TimeData timeData;	//存储计时相关数据

	// 游戏资源
	vector<IMAGE> blockImgs;        // 拼图图片资源
	IMAGE bkImg;					// 背景图
	//IMAGE bkImg;					// 背景图
	Sound sound_bgm;                // 背景音乐
	SoundBuffer buffer_bgm;         // 背景音乐缓冲区
	Sound sound_click;             // 点击音效
	SoundBuffer buffer_click;       // 点击音效缓冲区
	Sound sound_win;                // 胜利音效
	SoundBuffer buffer_win;         // 胜利音效缓冲区
	Dialogue* dialogue = nullptr;				//用于展示剧情的对话对象指针

	// 输入控制相关(暂时没明白)
	DWORD lastMoveTime = 0;         // 移动键最后触发时间(用于冷却)
	DWORD lastFunctionTime = 0;     // 功能键最后触发时间(用于冷却)
	const DWORD moveDelay = 150;    // 移动键冷却时间(毫秒)
	const DWORD functionDelay = 300;// 功能键冷却时间(毫秒)


	//构造函数（三个变量初始化，素材引入）
	Level(int tid, int tSSIZE, int tTmode);
	LevelResult virtual play();		//原来的main函数部分
	virtual void drawDialogue(string);		//关卡开始/结束时展示剧情对话用
	virtual void drawGame();				//绘制游戏界面

	void fadeImage(IMAGE* img, int x, int y, int fadeInTime, int stayTime, int fadeOutTime);//插入图片的淡入淡出
	void fadeInImage(IMAGE* img, int x, int y, int);
	void fadeOutImage(IMAGE* img, int x, int y, int);
protected:
	//游戏界面
	virtual void initBoard();				//初始化游戏板
	                                //绘制游戏界面我移到public中去了
	bool isWin();					//胜利判定
	void showWin();					//胜利结算
	void Gameopen();				//游戏启动界面

	//移动操作逻辑
	bool canMove(int row, int  col);	//检查点击方块是否可以移动
	bool handleMouse();					//鼠标点击处理
	bool handleKeyboard();				//键盘移动处理
	virtual int handleFunctionKeys();   //功能键处理函数

	void shuffleBoard();				//打乱游戏板
	virtual void moveTile(int row, int col);	//处理方块移动

};

//临时放置，测试用

class Level_TE :public Level {
public:
	Level_TE(int id, int SSIZE, int Tmode) :Level(id, SSIZE, Tmode) {
		alphaBoard.resize(SSIZE, vector<BYTE>(SSIZE, 255)); // 初始透明度255
	}
	LevelResult play() override;
	int handleFunctionKeys() override;
	void Debuff_jojo();
	void Shuffle(int n);
	void Buff_jojo(int n);

	void initBoard() override;
	void moveTile(int row, int col) override;
	void drawGame()override;

	vector<vector<BYTE>> alphaBoard;  // 每个拼图块的透明度
	const BYTE alphaDecrement = 20;   // 每次移动减少的透明度值
	const BYTE minAlpha = 50;         // 最小透明度
};
void initAnimations();
//extern Animation debuffAnimation;