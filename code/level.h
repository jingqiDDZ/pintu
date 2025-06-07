#pragma once
#include <ctime>
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
	sf::Sound killerQueen_de;
	sf::SoundBuffer Buffer_killerQueen_de;
	sf::Sound killerQueen;
	sf::SoundBuffer Buffer_killerQueen;
	sf::Sound kingCrimson;
	sf::SoundBuffer Buffer_kingCrimson;
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
	
	
	
	static bool gdiplusInitialized;
	static ULONG_PTR gdiplusToken;

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



class Level_3 :public Level {
public:
	Animation display;
	IMAGE all;

	void initAnimations() {

		display.init({ L"./assets/image/level/3/all.png" }, BLOCK_SIZE, BLOCK_SIZE, Animation::NON_BLOCKING, 1000, 100);

		// 加载动画资源
		if (!display.loadFrames()) {
			MessageBox(GetHWnd(), _T("图片动画资源加载失败"), _T("错误"), MB_OK);
		}
	}

	Level_3(int id, int SSIZE, int Tmode) :Level(id, SSIZE, Tmode) {
		loadimage(&all, _T("./assets/image/level/3/all.png"), BLOCK_SIZE, BLOCK_SIZE);
		initAnimations();
	}

	LevelResult play() override {
		BeginBatchDraw();	//双缓冲防止游戏闪烁
		Gameopen();			//绘制开始界面
		FlushBatchDraw();   //刷新缓冲区，显示初始界面

		// 步骤1：等待所有按键松开
		bool keyStillDown = true;
		while (keyStillDown) {
			keyStillDown = false;
			for (int vKey = 8; vKey <= 255; vKey++) {
				if (GetAsyncKeyState(vKey) & 0x8000) {
					keyStillDown = true;
					break;
				}
			}
			Sleep(30);
		}

		// 步骤2：等待玩家按下任意键
		while (true) {
			// 检查ESC
			if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
				return LevelResult::Exit;
			}
			// 检查任意键
			for (int vKey = 8; vKey <= 255; vKey++) {
				if (GetAsyncKeyState(vKey) & 0x8000) {
					//展示对话剧情之后再开始游戏
					string tmppath = "./assets/text/level/" + to_string(id) + "/dialogue.json";
					drawDialogue(tmppath);
					FlushBatchDraw();
					goto CONTINUE_GAME;
				}
			}
			Sleep(30);
		}

	CONTINUE_GAME:

		shuffleBoard();
		if (Tmode == 0) {
			time(&timeData.startTime);
		}
		else if (Tmode == 1) {
			countdownData.startTime = chrono::system_clock::now();
		}

		bool bgm_start = false;			//负责第一次进入循环时开始播放bgm

		while (true) {

			if (Tmode == 1) {
				auto now = std::chrono::system_clock::now();
				chrono::duration<double> elapsed_Re = now - countdownData.startTime;
				countdownData.remaining = countdownData.totalTime - static_cast<int>(elapsed_Re.count());

				if (countdownData.remaining <= 0 && !isWin()) {
					countdownData.isTimeout = true;
					countdownData.remaining = 0;
				}

				if (countdownData.remaining <= 10 && countdownData.Played_count == false) {
					countdownData.sound.play();
					countdownData.Played_count = true;
				}
			}//倒计时

			if (!bgm_start) {
				bgm_start = true;
				sound_bgm.play();
			}

			drawGame();

			// 更新非阻塞动画
			if (display.isPlaying() && display.getType() == Animation::NON_BLOCKING) {
				display.updateNonBlocking();
			}

			/*// 检查是否有阻塞动画在播放（如debuffAnimation）
			bool isBlockingAnimation = debuffAnimation.isPlaying() &&
				(debuffAnimation.getType() == Animation::BLOCKING);
			*/
			// 非动画期间处理输入
			//if (!isBlockingAnimation) {
				if (handleMouse() || handleKeyboard()) {
					sound_click.play();
				}

				//处理功能键
				int funcReturn = handleFunctionKeys();
				if (funcReturn == 1) {
					return LevelResult::Exit;
				}
			//}

			//胜利检测
			if (isWin()) {
				if (Tmode == 0) {
					time(&timeData.endTime);
					isWinning = true;
				}
				else  if (Tmode == 1) {
					// 计算用时
					auto endTime_Re = chrono::system_clock::now();
					countdownData.used = endTime_Re - countdownData.startTime;
				}
				cout << "WOW~~ isWin!" << endl;
				showWin();
				//结算之后，展示对话剧情再结束游戏
				string tmppath = "./assets/text/level/" + to_string(id) + "/dialogue2.json";
				drawDialogue(tmppath);
				FlushBatchDraw();
				return LevelResult::Win;

			}

			/*if (isBlockingAnimation) {
				// 阻塞动画有自己的绘制逻辑，跳过常规绘制
				continue;
			}*/

			//超时检测
			if (Tmode == 1 && countdownData.isTimeout) {
				// 绘制超时界面
				setbkcolor(RGB(240, 240, 240));
				cleardevice();

				settextcolor(RED);
				settextstyle(40, 0, _T("宋体"));
				outtextxy(SSIZE * 50 - 40, SSIZE * 100 + 230, _T("时间耗尽!"));

				settextstyle(20, 0, _T("宋体"));
				outtextxy(150, SSIZE * 100 + 270, _T("按R重新开始"));
				outtextxy(150, SSIZE * 100 + 300, _T("ESC退出游戏"));

				FlushBatchDraw();

				// 非阻塞按键检测
				while (true) {
					// 检测R键
					if (GetAsyncKeyState('R') & 0x8000) {
						initBoard();
						shuffleBoard();
						moves = 0;
						countdownData.Played_count = false;
						countdownData.startTime = chrono::system_clock::now();
						countdownData.isTimeout = false;
						break;
					}
					// 检测ESC键
					else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
						return LevelResult::Exit;
					}

					// 避免CPU占用过高		牛逼，还考虑这种，真是太棒啦~
					Sleep(50);
				}
				continue; // 跳过后续游戏逻辑
			}//倒计时超时

			FlushBatchDraw();
			Sleep(0.1);
		}//游戏运行

		//closegraph();		游戏结束不关闭窗口
		return LevelResult::Exit;
	}


	int handleFunctionKeys() override {
		DWORD currentTime = GetTickCount();

		// 检查是否在冷却时间内
		if (currentTime - lastFunctionTime < functionDelay) {
			return 0;
		}

		// 检测 R 键：重启游戏
		if (GetAsyncKeyState('R') & 0x8000) {
			initBoard();
			shuffleBoard();
			moves = 0;
			if (Tmode == 0) {
				time(&timeData.startTime);
				isWinning = false;
			}
			else if (Tmode == 1) {
				countdownData.startTime = std::chrono::system_clock::now();
				countdownData.isTimeout = false;
			}
			lastFunctionTime = currentTime;
		}
		// 检测 ESC 键：退出游戏
		else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
			return 1;
		}

		// 检测 Z 键：测试胜利
		else if (GetAsyncKeyState('Z') & 0x8000) {
			if (Tmode == 0) {
				time(&timeData.endTime);
				isWinning = true;
			}
			else  if (Tmode == 1) {
				// 计算用时
				auto endTime_Re = chrono::system_clock::now();
				countdownData.used = endTime_Re - countdownData.startTime;
			}
			cout << "WOW~~ isWin!" << endl;
			showWin();
			cout << "showWin finished" << endl;


			lastFunctionTime = currentTime;
		}

		
		//检测C键:展示图片
		if (GetAsyncKeyState('C') & 0x8000) {
			if (!display.isPlaying()) {
				int X = WD_width / 4 * 3;
				int Y = WD_height / 4 * 3;
				display.setStayDuration(5000);
				display.startNonBlocking(X, Y, X, Y);
			}
			lastFunctionTime = currentTime;
		}
	}
};

class Level_4 :public Level
{
public:
	Animation display;
	IMAGE all;

	void initAnimations() {

		display.init({ L"./assets/image/level/4/all.png" }, BLOCK_SIZE, BLOCK_SIZE, Animation::NON_BLOCKING, 1000, 100);

		// 加载动画资源
		if (!display.loadFrames()) {
			MessageBox(GetHWnd(), _T("图片动画资源加载失败"), _T("错误"), MB_OK);
		}
	}

	Level_4(int id, int SSIZE, int Tmode) :Level(id, SSIZE, Tmode) {
		alphaBoard.resize(SSIZE, vector<BYTE>(SSIZE, 255)); // 初始透明度255
		loadimage(&all, _T("./assets/image/level/4/all.png"), BLOCK_SIZE, BLOCK_SIZE);
		initAnimations();
		loadSoundClip("./assets/audio/Buff_jojo.wav", killerQueen, Buffer_killerQueen);//发动败者食尘成功的音乐
		loadSoundClip("./assets/audio/Buff_jojo_de.wav", killerQueen_de, Buffer_killerQueen_de);//发动败者食尘失败的音乐

	}


	LevelResult play() override {
		BeginBatchDraw();	//双缓冲防止游戏闪烁
		Gameopen();			//绘制开始界面
		FlushBatchDraw();   //刷新缓冲区，显示初始界面

		// 步骤1：等待所有按键松开
		bool keyStillDown = true;
		while (keyStillDown) {
			keyStillDown = false;
			for (int vKey = 8; vKey <= 255; vKey++) {
				if (GetAsyncKeyState(vKey) & 0x8000) {
					keyStillDown = true;
					break;
				}
			}
			Sleep(30);
		}

		// 步骤2：等待玩家按下任意键
		while (true) {
			// 检查ESC
			if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
				return LevelResult::Exit;
			}
			// 检查任意键
			for (int vKey = 8; vKey <= 255; vKey++) {
				if (GetAsyncKeyState(vKey) & 0x8000) {
					//展示对话剧情之后再开始游戏
					string tmppath = "./assets/text/level/" + to_string(id) + "/dialogue.json";
					drawDialogue(tmppath);
					FlushBatchDraw();
					goto CONTINUE_GAME;
				}
			}
			Sleep(30);
		}

	CONTINUE_GAME:

		shuffleBoard();
		if (Tmode == 0) {
			time(&timeData.startTime);
		}
		else if (Tmode == 1) {
			countdownData.startTime = chrono::system_clock::now();
		}



		initAnimations();
		bool bgm_start = false;			//负责第一次进入循环时开始播放bgm



		while (true) {

			if (Tmode == 1) {
				auto now = std::chrono::system_clock::now();
				chrono::duration<double> elapsed_Re = now - countdownData.startTime;
				countdownData.remaining = countdownData.totalTime - static_cast<int>(elapsed_Re.count());

				if (countdownData.remaining <= 0 && !isWin()) {
					countdownData.isTimeout = true;
					countdownData.remaining = 0;
				}

				if (countdownData.remaining <= 10 && countdownData.Played_count == false) {
					countdownData.sound.play();
					countdownData.Played_count = true;
				}
			}//倒计时

			if (!bgm_start) {
				bgm_start = true;
				sound_bgm.play();
			}

			drawGame();

			// 更新非阻塞动画
			if (display.isPlaying() && display.getType() == Animation::NON_BLOCKING) {
				display.updateNonBlocking();
			}

			// 检查是否有阻塞动画在播放（如debuffAnimation）
			/*bool isBlockingAnimation = debuffAnimation.isPlaying() &&
				(debuffAnimation.getType() == Animation::BLOCKING);*/

			// 非动画期间处理输入
			//if (!isBlockingAnimation) {
				if (handleMouse() || handleKeyboard()) {
					sound_click.play();
				}

				//处理功能键
				int funcReturn = handleFunctionKeys();
				if (funcReturn == 1) {
					return LevelResult::Exit;
				}
			//}

			//胜利检测
			if (isWin()) {
				if (Tmode == 0) {
					time(&timeData.endTime);
					isWinning = true;
				}
				else  if (Tmode == 1) {
					// 计算用时
					auto endTime_Re = chrono::system_clock::now();
					countdownData.used = endTime_Re - countdownData.startTime;
				}
				cout << "WOW~~ isWin!" << endl;
				showWin();
				//结算之后，展示对话剧情再结束游戏
				string tmppath = "./assets/text/level/" + to_string(id) + "/dialogue2.json";
				drawDialogue(tmppath);
				FlushBatchDraw();
				return LevelResult::Win;

			}

			/*if (isBlockingAnimation) {
				// 阻塞动画有自己的绘制逻辑，跳过常规绘制
				continue;
			}*/

			//超时检测
			if (Tmode == 1 && countdownData.isTimeout) {
				// 绘制超时界面
				setbkcolor(RGB(240, 240, 240));
				cleardevice();

				settextcolor(RED);
				settextstyle(40, 0, _T("宋体"));
				outtextxy(SSIZE * 50 - 40, SSIZE * 100 + 230, _T("时间耗尽!"));

				settextstyle(20, 0, _T("宋体"));
				outtextxy(150, SSIZE * 100 + 270, _T("按R重新开始"));
				outtextxy(150, SSIZE * 100 + 300, _T("ESC退出游戏"));

				FlushBatchDraw();

				// 非阻塞按键检测
				while (true) {
					// 检测R键
					if (GetAsyncKeyState('R') & 0x8000) {
						initBoard();
						shuffleBoard();
						moves = 0;
						countdownData.Played_count = false;
						countdownData.startTime = chrono::system_clock::now();
						countdownData.isTimeout = false;
						break;
					}
					// 检测ESC键
					else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
						return LevelResult::Exit;
					}

					// 避免CPU占用过高		牛逼，还考虑这种，真是太棒啦~
					Sleep(50);
				}
				continue; // 跳过后续游戏逻辑
			}//倒计时超时

			FlushBatchDraw();
			Sleep(0.1);
		}//游戏运行

		//closegraph();		游戏结束不关闭窗口
		return LevelResult::Exit;
	}


	int handleFunctionKeys() override {
		DWORD currentTime = GetTickCount();

		// 检查是否在冷却时间内
		if (currentTime - lastFunctionTime < functionDelay) {
			return 0;
		}

		// 检测 R 键：重启游戏
		if (GetAsyncKeyState('R') & 0x8000) {
			initBoard();
			shuffleBoard();
			moves = 0;
			if (Tmode == 0) {
				time(&timeData.startTime);
				isWinning = false;
			}
			else if (Tmode == 1) {
				countdownData.startTime = std::chrono::system_clock::now();
				countdownData.isTimeout = false;
			}
			lastFunctionTime = currentTime;
		}
		// 检测 ESC 键：退出游戏
		else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
			return 1;
		}

		// 检测 Z 键：测试胜利
		else if (GetAsyncKeyState('Z') & 0x8000) {
			if (Tmode == 0) {
				time(&timeData.endTime);
				isWinning = true;
			}
			else  if (Tmode == 1) {
				// 计算用时
				auto endTime_Re = chrono::system_clock::now();
				countdownData.used = endTime_Re - countdownData.startTime;
			}
			cout << "WOW~~ isWin!" << endl;
			showWin();
			cout << "showWin finished" << endl;


			lastFunctionTime = currentTime;
		}

		// 检测 V 键：回退操作
		else if (GetAsyncKeyState('V') & 0x8000) {
			if (history.size() >= 3) {
				Buff_jojo(3);
			}
			else {
				killerQueen_de.play();
			}
			lastFunctionTime = currentTime;
		}

		//检测C键:展示图片
		else if (GetAsyncKeyState('C') & 0x8000) {
			if (!display.isPlaying()) {
				int X = WD_width / 4 * 3;
				int Y = WD_height / 4 * 3;
				display.setStayDuration(5000);
				display.startNonBlocking(X, Y, X, Y);
			}
			lastFunctionTime = currentTime;
		}
	}


	void Buff_jojo(int n) {
		killerQueen.play();
		for (int i = 0;i < n;i++) {
			if (!history.empty()) {
				// 恢复上一个状态
				board = history.back();
				history.pop_back();
				moves--;

				// 更新空白块位置
				for (int i = 0; i < SSIZE; i++) {
					for (int j = 0; j < SSIZE; j++) {
						if (board[i][j] == 0) {
							emptyRow = i;
							emptyCol = j;
						}
					}
				}
			}

			//刷新

			drawGame();
			FlushBatchDraw();
		}
	}

	void initBoard() override {
		Level::initBoard();
		// 重置所有透明度为255（完全不透明）
		for (int i = 0; i < SSIZE; i++) {
			for (int j = 0; j < SSIZE; j++) {
				alphaBoard[i][j] = 255;
			}
		}
	}


	void moveTile(int row, int col) override {
		if (!canMove(row, col)) return;

		// 保存历史状态
		history.push_back(board);

		// 更新透明度 (仅移动的方块)
		if (alphaBoard[row][col] > minAlpha) {
			alphaBoard[row][col] =
				(alphaBoard[row][col] - alphaDecrement < minAlpha) ?
				minAlpha : (alphaBoard[row][col] - alphaDecrement);
		}

		// 保存移动前的空白位置
		int oldEmptyRow = emptyRow;
		int oldEmptyCol = emptyCol;

		// 移动方块
		swap(board[emptyRow][emptyCol], board[row][col]);
		swap(alphaBoard[emptyRow][emptyCol], alphaBoard[row][col]);

		// 更新空白位置
		emptyRow = row;
		emptyCol = col;
		moves++;

		// +++ 新增：检查移动后的方块是否在正确位置 +++
		int movedValue = board[oldEmptyRow][oldEmptyCol];  // 被移动的方块值
		if (movedValue != 0) {
			// 计算正确位置（行和列）
			int correctRow = (movedValue - 1) / SSIZE;
			int correctCol = (movedValue - 1) % SSIZE;

			// 如果当前在正确位置，重置透明度
			if (oldEmptyRow == correctRow && oldEmptyCol == correctCol) {
				alphaBoard[oldEmptyRow][oldEmptyCol] = 255;  // 完全透明
			}
		}
	}


	void drawGame()override {
		setbkcolor(RGB(240, 240, 240));
		cleardevice();


		if (!Level::gdiplusInitialized) {
			Gdiplus::GdiplusStartupInput input;
			Gdiplus::GdiplusStartup(&Level::gdiplusToken, &input, NULL);
			Level::gdiplusInitialized = true;
		}

		// 绘制标题和移动次数（保持不变）
		settextcolor(BLACK);
		settextstyle(30, 0, _T("宋体"));
		outtextxy(SSIZE * 50, 10, _T("数字华容道"));

		TCHAR movesText[50];
		_stprintf_s(movesText, _T("移动次数: %d"), moves);
		settextstyle(20, 0, _T("宋体"));
		outtextxy(SSIZE * 50 + 50, 50, movesText);

		//绘制计时时间
		if (Tmode == 0) {
			time_t currentTime;
			if (isWinning) {
				currentTime = timeData.endTime;
			}
			else {
				time(&currentTime);
			}
			int elapsed = difftime(currentTime, timeData.startTime);
			int minutes = elapsed / 60;
			int seconds = elapsed % 60;

			TCHAR timeText[50];
			_stprintf_s(timeText, _T("时间: %02d:%02d"), minutes, seconds);
			outtextxy(SSIZE * 50 + 50, 80, timeText);  // 调整位置避免重叠
		}
		else if (Tmode == 1) {
			int minutes = countdownData.remaining / 60;
			int seconds = countdownData.remaining % 60;

			TCHAR timeText[50];
			_stprintf_s(timeText, _T("剩余时间: %02d:%02d"), minutes, seconds);
			settextcolor(RED);  // 用红色强调时间
			outtextxy(SSIZE * 50 + 50, 80, timeText);
			settextcolor(BLACK); // 恢复默认颜色
		}

		// 绘制游戏方块（带透明度）
		for (int i = 0; i < SSIZE; i++) {
			for (int j = 0; j < SSIZE; j++) {
				int value = board[i][j];
				if (value == 0) continue; // 空白块不绘制

				int x = MARGIN + j * BLOCK_SIZE;
				int y = MARGIN + i * BLOCK_SIZE + 80;
				BYTE alpha = alphaBoard[i][j];

				// 使用GDI+绘制带透明度的图像
				HDC hdc = GetImageHDC();
				Graphics graphics(hdc);

				// 创建临时位图
				Bitmap bmp(BLOCK_SIZE, BLOCK_SIZE, PixelFormat32bppARGB);
				BitmapData bmpData;
				Gdiplus::Rect rect(0, 0, BLOCK_SIZE, BLOCK_SIZE);
				bmp.LockBits(&rect, ImageLockModeWrite, PixelFormat32bppARGB, &bmpData);

				// 获取原始图片数据
				DWORD* pSrc = (DWORD*)GetImageBuffer(&blockImgs[value]);
				BYTE* pDst = (BYTE*)bmpData.Scan0;
				for (int y = 0; y < BLOCK_SIZE; y++) {
					for (int x = 0; x < BLOCK_SIZE; x++) {
						int srcIdx = y * BLOCK_SIZE + x;
						DWORD srcColor = pSrc[srcIdx];

						// 提取BGRA分量 (EasyX格式)
						BYTE b = (srcColor >> 0) & 0xFF;
						BYTE g = (srcColor >> 8) & 0xFF;
						BYTE r = (srcColor >> 16) & 0xFF;
						BYTE originalAlpha = (srcColor >> 24) & 0xFF;

						// 计算最终透明度
						BYTE finalAlpha = (originalAlpha * alpha) / 255;

						// 转换为ARGB格式 (GDI+需要)
						*pDst++ = b;  // Blue
						*pDst++ = g;  // Green
						*pDst++ = r;  // Red
						*pDst++ = finalAlpha; // Alpha
					}
					// 跳过行填充字节
					pDst += bmpData.Stride - BLOCK_SIZE * 4;
				}
				bmp.UnlockBits(&bmpData);

				// 绘制到屏幕
				graphics.DrawImage(&bmp, x, y);

				// 绘制边框
				setlinecolor(BLACK);
				rectangle(x, y, x + BLOCK_SIZE, y + BLOCK_SIZE);
			}
		}

		// 绘制操作说明（保持不变）
		settextstyle(16, 0, _T("宋体"));
		outtextxy(50, SSIZE * 100 + 150, _T("操作说明:"));
		outtextxy(50, SSIZE * 100 + 170, _T("鼠标点击 - 移动方块"));
		outtextxy(50, SSIZE * 100 + 190, _T("R - 重新开始"));
		outtextxy(50, SSIZE * 100 + 210, _T("ESC - 退出游戏"));
	}

	vector<vector<BYTE>> alphaBoard;  // 每个拼图块的透明度
	const BYTE alphaDecrement = 30;   // 每次移动减少的透明度值
	const BYTE minAlpha = 25;         // 最小透明度
};

class Level_5 : public Level{
public:
	Animation display;
	Animation water1;
	Animation water2;
	Animation water3;
	IMAGE all;
	Sound Guqin;
	SoundBuffer Buffer_Guqin;
	const static int Prob = 40;
	int stepsize = 5;
	int Minvalue_Prob = 5;
	static int value_5;

	void initAnimations() {

		display.init({ L"./assets/image/level/5/all.png" }, BLOCK_SIZE, BLOCK_SIZE, Animation::NON_BLOCKING, 1000, 100);
		water1.init({ L"./assets/anim/water1.png" }, 375, 237, Animation::NON_BLOCKING, 300, 100);
		water2.init({ L"./assets/anim/water2.png" }, 375, 237, Animation::NON_BLOCKING, 300, 100);
		water3.init({ L"./assets/anim/water3.png" }, 375, 237, Animation::NON_BLOCKING, 300, 100);


		// 加载动画资源
		if (!display.loadFrames()) {
			MessageBox(GetHWnd(), _T("图片动画资源加载失败"), _T("错误"), MB_OK);
		}
		if (!water1.loadFrames()) {
			MessageBox(GetHWnd(), _T("涟漪1动画资源加载失败"), _T("错误"), MB_OK);
		}
		if (!water2.loadFrames()) {
			MessageBox(GetHWnd(), _T("涟漪2动画资源加载失败"), _T("错误"), MB_OK);
		}
		if (!water3.loadFrames()) {
			MessageBox(GetHWnd(), _T("涟漪3动画资源加载失败"), _T("错误"), MB_OK);
		}
	}

	Level_5(int id, int SSIZE, int Tmode) :Level(id, SSIZE, Tmode) {
		loadimage(&all, _T("./assets/image/level/5/all.png"), BLOCK_SIZE, BLOCK_SIZE);
		initAnimations();
		loadSoundClip("./assets/audio/Buff_jojo.wav", killerQueen, Buffer_killerQueen);//发动败者食尘成功的音乐
		loadSoundClip("./assets/audio/Buff_jojo_de.wav", killerQueen_de, Buffer_killerQueen_de);//发动败者食尘失败的音乐
		loadSoundClip("./assets/audio/Guqin.wav", Guqin, Buffer_Guqin);//古琴
	}

	void moveTile(int row,int col)override{
		if (canMove(row, col)) {

			// 保存当前状态到历史记录
			history.push_back(board);
			// 保持最多50步历史记录
			if (history.size() > 50) history.erase(history.begin());

			board[emptyRow][emptyCol] = board[row][col];
			board[row][col] = 0;
			emptyRow = row;
			emptyCol = col;
			moves++;
			Level_5::value_5 = (value_5 - stepsize) < Minvalue_Prob ? Minvalue_Prob : (value_5 - stepsize);
			int r = rand() % value_5;
			if (r == 1) {
				Guqin.play();
				int P = rand() % 3;
				switch (P) {
				case 0: {
					water1.setAlpha(50);
					water2.setAlpha(50);
					water3.setAlpha(50);
					break;
				}
				case 1: {
					water1.setAlpha(150);
					water2.setAlpha(150);
					water3.setAlpha(150);
					break;
				}
				case 2: {
					water1.setAlpha(250);
					water2.setAlpha(250);
					water3.setAlpha(250);
					break;
				}
				}
				int animWidth = 375;
				int animHeight = 237;
				int h = rand() % 3;
				switch (h) {
				case 0: {
					int w = rand() % 4;
					switch (w) {
					case 0:water1.startNonBlocking(0, 0, 0, 0);break;
					case 1:water1.startNonBlocking(0, WD_height-animHeight, 0, WD_height-animHeight);break;
					case 2:water1.startNonBlocking(WD_width-animWidth, 0, WD_width-animWidth, 0);break;
					case 3:water1.startNonBlocking(WD_width-animWidth, WD_height-animHeight, WD_width-animWidth, WD_height-animHeight);break;
					}
				}
				case 1: {
					int w = rand() % 4;
					switch (w) {
					case 0:water2.startNonBlocking(0, 0, 0, 0);break;
					case 1:water2.startNonBlocking(0, WD_height - animHeight, 0, WD_height - animHeight);break;
					case 2:water2.startNonBlocking(WD_width - animWidth, 0, WD_width - animWidth, 0);break;
					case 3:water2.startNonBlocking(WD_width - animWidth, WD_height - animHeight, WD_width - animWidth, WD_height - animHeight);break;
					}
				}
				case 2: {
					int w = rand() % 4;
					switch (w) {
					case 0:water3.startNonBlocking(0, 0, 0, 0);break;
					case 1:water3.startNonBlocking(0, WD_height - animHeight, 0, WD_height - animHeight);break;
					case 2:water3.startNonBlocking(WD_width - animWidth, 0, WD_width - animWidth, 0);break;
					case 3:water3.startNonBlocking(WD_width - animWidth, WD_height - animHeight, WD_width - animWidth, WD_height - animHeight);break;
					}
				}

				}
			}
			if (value_5 == Minvalue_Prob) value_5 = Prob;
		}
	}

	LevelResult play() override {
		BeginBatchDraw();	//双缓冲防止游戏闪烁
		Gameopen();			//绘制开始界面
		FlushBatchDraw();   //刷新缓冲区，显示初始界面

		// 步骤1：等待所有按键松开
		bool keyStillDown = true;
		while (keyStillDown) {
			keyStillDown = false;
			for (int vKey = 8; vKey <= 255; vKey++) {
				if (GetAsyncKeyState(vKey) & 0x8000) {
					keyStillDown = true;
					break;
				}
			}
			Sleep(30);
		}

		// 步骤2：等待玩家按下任意键
		while (true) {
			// 检查ESC
			if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
				return LevelResult::Exit;
			}
			// 检查任意键
			for (int vKey = 8; vKey <= 255; vKey++) {
				if (GetAsyncKeyState(vKey) & 0x8000) {
					goto CONTINUE_GAME;
				}
			}
			Sleep(30);
		}

	CONTINUE_GAME:

		shuffleBoard();
		if (Tmode == 0) {
			time(&timeData.startTime);
		}
		else if (Tmode == 1) {
			countdownData.startTime = chrono::system_clock::now();
		}



		initAnimations();
		bool bgm_start = false;			//负责第一次进入循环时开始播放bgm



		while (true) {

			if (Tmode == 1) {
				auto now = std::chrono::system_clock::now();
				chrono::duration<double> elapsed_Re = now - countdownData.startTime;
				countdownData.remaining = countdownData.totalTime - static_cast<int>(elapsed_Re.count());

				if (countdownData.remaining <= 0 && !isWin()) {
					countdownData.isTimeout = true;
					countdownData.remaining = 0;
				}

				if (countdownData.remaining <= 10 && countdownData.Played_count == false) {
					countdownData.sound.play();
					countdownData.Played_count = true;
				}
			}//倒计时

			if (!bgm_start) {
				bgm_start = true;
				sound_bgm.play();
			}

			drawGame();

			// 更新非阻塞动画
			if (display.isPlaying() && display.getType() == Animation::NON_BLOCKING) {
				display.updateNonBlocking();
			}
			if (water1.isPlaying() && water1.getType() == Animation::NON_BLOCKING) {
				water1.updateNonBlocking();
			}
			if (water2.isPlaying() && water2.getType() == Animation::NON_BLOCKING) {
				water2.updateNonBlocking();
			}
			if (water3.isPlaying() && water3.getType() == Animation::NON_BLOCKING) {
				water3.updateNonBlocking();
			}

			// 检查是否有阻塞动画在播放（如debuffAnimation）
			/*bool isBlockingAnimation = debuffAnimation.isPlaying() &&
				(debuffAnimation.getType() == Animation::BLOCKING);*/

				// 非动画期间处理输入
				//if (!isBlockingAnimation) {
			if (handleMouse() || handleKeyboard()) {
				sound_click.play();
			}

			//处理功能键
			int funcReturn = handleFunctionKeys();
			if (funcReturn == 1) {
				return LevelResult::Exit;
			}
			//}

			//胜利检测
			if (isWin()) {
				if (Tmode == 0) {
					time(&timeData.endTime);
					isWinning = true;
				}
				else  if (Tmode == 1) {
					// 计算用时
					auto endTime_Re = chrono::system_clock::now();
					countdownData.used = endTime_Re - countdownData.startTime;
				}
				cout << "WOW~~ isWin!" << endl;
				showWin();

				continue;
			}

			/*if (isBlockingAnimation) {
				// 阻塞动画有自己的绘制逻辑，跳过常规绘制
				continue;
			}*/

			//超时检测
			if (Tmode == 1 && countdownData.isTimeout) {
				// 绘制超时界面
				setbkcolor(RGB(240, 240, 240));
				cleardevice();

				settextcolor(RED);
				settextstyle(40, 0, _T("宋体"));
				outtextxy(SSIZE * 50 - 40, SSIZE * 100 + 230, _T("时间耗尽!"));

				settextstyle(20, 0, _T("宋体"));
				outtextxy(150, SSIZE * 100 + 270, _T("按R重新开始"));
				outtextxy(150, SSIZE * 100 + 300, _T("ESC退出游戏"));

				FlushBatchDraw();

				// 非阻塞按键检测
				while (true) {
					// 检测R键
					if (GetAsyncKeyState('R') & 0x8000) {
						initBoard();
						shuffleBoard();
						moves = 0;
						countdownData.Played_count = false;
						countdownData.startTime = chrono::system_clock::now();
						countdownData.isTimeout = false;
						break;
					}
					// 检测ESC键
					else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
						return LevelResult::Exit;
					}

					// 避免CPU占用过高		牛逼，还考虑这种，真是太棒啦~
					Sleep(50);
				}
				continue; // 跳过后续游戏逻辑
			}//倒计时超时

			FlushBatchDraw();
			Sleep(0.1);
		}//游戏运行

		//closegraph();		游戏结束不关闭窗口
		return LevelResult::Exit;
	}
};

class Level_6 :public Level {
public:
	Animation display;
	Animation debuffAnimation;
	IMAGE all;
	const static int Prob=40;
	int stepsize = 5;
	int Minvalue_Prob=5;

	void initAnimation() {
		display.init({ L"./assets/image/level/6/all.png" }, BLOCK_SIZE, BLOCK_SIZE, Animation::NON_BLOCKING, 1000, 100);
		debuffAnimation.init(
			{
			L"./assets/anim/te.png",
			L"./assets/anim/te1.png",
			L"./assets/anim/te2.png",
			L"./assets/anim/te3.png",
			L"./assets/anim/te4.png",
			L"./assets/anim/te5.png",
			L"./assets/anim/te6.png",
			L"./assets/anim/te7.png",
			L"./assets/anim/te8.png",
			L"./assets/anim/te9.png"
			},
			400, 300, Animation::BLOCKING, 1000, 100
		);
		
			
		// 加载动画资源
		if (!display.loadFrames()) {
			MessageBox(GetHWnd(), _T("图片动画资源加载失败"), _T("错误"), MB_OK);
		}
		if (!debuffAnimation.loadFrames()) {
			MessageBox(GetHWnd(), _T("Debuff动画资源加载失败"), _T("错误"), MB_OK);
		}
	}

	Level_6(int id, int SSIZE, int Tmode) :Level(id, SSIZE, Tmode) {
		loadimage(&all, _T("./assets/image/level/6/all.png"), BLOCK_SIZE, BLOCK_SIZE);
		initAnimation();
		loadSoundClip("./assets/audio/Buff_jojo.wav", killerQueen, Buffer_killerQueen);//发动败者食尘成功的音乐
		loadSoundClip("./assets/audio/Buff_jojo_de.wav", killerQueen_de, Buffer_killerQueen_de);//发动败者食尘失败的音乐
		loadSoundClip("./assets/audio/Debuff_jojo.wav", kingCrimson, Buffer_kingCrimson);
		srand((unsigned)time(nullptr));


	}

	int handleFunctionKeys() override;

	LevelResult play() override;

	void Buff_jojo(int n);

	void Debuff_jojo();

	void Shuffle(int n);

	void moveTile(int row, int col)override;
};
