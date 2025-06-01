#include "level.h"

Animation debuffAnimation;
sf::Sound killerQueen_de;
sf::SoundBuffer Buffer_killerQueen_de;
sf::Sound killerQueen;
sf::SoundBuffer Buffer_killerQueen;

sf::Sound kingCrimson;
sf::SoundBuffer Buffer_kingCrimson;


LevelResult Level_TE::play() {
	//SSIZE = 3;
	//board.resize(SSIZE, vector<int>(SSIZE));
	//blockImgs.resize(SSIZE * SSIZE);
	//initgraph(SSIZE * 100 + 400, SSIZE * 100 + 600);//游戏板大小		这一段我先注释，避免打开新窗口

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

	//按任意键进入游戏
	//initBoard();
	shuffleBoard();
	if (Tmode == 0) {
		time(&timeData.startTime);
	}
	else if (Tmode == 1) {
		countdownData.startTime = chrono::system_clock::now();
	}

	loadSoundClip("./assets/audio/Debuff_jojo.wav", kingCrimson, Buffer_kingCrimson);

	loadSoundClip("./assets/audio/Buff_jojo.wav", killerQueen, Buffer_killerQueen);//发动败者食尘成功的音乐

	loadSoundClip("./assets/audio/Buff_jojo_de.wav", killerQueen_de, Buffer_killerQueen_de);//发动败者食尘失败的音乐



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

		bool isBlockingAnimation = debuffAnimation.isPlaying() &&
			(debuffAnimation.getType() == Animation::BLOCKING); // 检查是否有阻塞动画在播放

		// 非动画期间处理输入
		if (!isBlockingAnimation) {
			if (handleMouse() || handleKeyboard()) {
				sound_click.play();
			}

			//处理功能键		这里还要改，推出窗口之类的问题
			int funcReturn = handleFunctionKeys();
			if (funcReturn == 1) {
				return LevelResult::Exit;
			}
		}

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

		if (isBlockingAnimation) {
			// 阻塞动画有自己的绘制逻辑，跳过常规绘制
			continue;
		}

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

		drawGame();
		FlushBatchDraw();
		Sleep(0.1);
	}//游戏运行

	//closegraph();		游戏结束不关闭窗口
	return LevelResult::Exit;
}

int Level_TE::handleFunctionKeys() {
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

	
	//检测 X 键：随机移动
	else if (GetAsyncKeyState('X') & 0x8000) {
		Debuff_jojo();
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
	
}





void Level_TE::Buff_jojo(int n) {
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



void initAnimations() {
	// 初始化debuff动画
	debuffAnimation.init(
		{
			L"./assets/anim/0.png",
			L"./assets/anim/1.png",
			L"./assets/anim/2.png",
			L"./assets/anim/3.png",
			L"./assets/anim/4.png",
			L"./assets/anim/5.png",
			L"./assets/anim/6.png",
			L"./assets/anim/7.png",
			L"./assets/anim/8.png",
			L"./assets/anim/9.png"
		},
		400, 300, Animation::BLOCKING, 1000, 100
	);

	// 加载动画资源
	if (!debuffAnimation.loadFrames()) {
		MessageBox(GetHWnd(), _T("Debuff动画资源加载失败"), _T("错误"), MB_OK);
	}
}

void Level_TE::Shuffle(int times) {
	srand((unsigned)time(NULL));
	int preDir[1][2];//先前移动的反方向
	int last = -1;//初始没有先前移动方向
	// 预生成所有可能的合法移动对
	for (int i = 0; i < times; i++) {
		// 获取当前空白块周围可交换的位置
		int neighbors[4][2] = {
			{emptyRow - 1, emptyCol}, // 上
			{emptyRow + 1, emptyCol}, // 下
			{emptyRow, emptyCol - 1}, // 左
			{emptyRow, emptyCol + 1}  // 右
		};

		// 收集所有合法的邻居位置
		int validNeighbors[4][2];
		int count = 0;
		for (int j = 0; j < 4; j++) {
			int r = neighbors[j][0];
			int c = neighbors[j][1];
			if (last == -1) {
				if (r >= 0 && r < SSIZE && c >= 0 && c < SSIZE) {
					validNeighbors[count][0] = r;
					validNeighbors[count][1] = c;
					count++;
				}
			}
			else {
				if (r >= 0 && r < SSIZE && c >= 0 && c < SSIZE &&
					r != preDir[0][0] && c != preDir[0][1]) {
					validNeighbors[count][0] = r;
					validNeighbors[count][1] = c;
					count++;
				}
			}
		}
		last = 1;
		// 随机选择一个邻居进行交换
		if (count > 0) {
			int idx = rand() % count;
			int targetRow = validNeighbors[idx][0];
			int targetCol = validNeighbors[idx][1];
			preDir[0][0] = emptyRow;
			preDir[0][1] = emptyCol;

			// 保存当前状态到历史记录
			history.push_back(board);
			// 保持最多50步历史记录
			if (history.size() > 50) history.erase(history.begin());

			// 执行交换（模拟移动）
			board[emptyRow][emptyCol] = board[targetRow][targetCol];
			board[targetRow][targetCol] = 0;
			emptyRow = targetRow;
			emptyCol = targetCol;
			moves++;
		}
	}
}

void Level_TE::Debuff_jojo() {
	kingCrimson.play();

	debuffAnimation.setAlpha(50);

	debuffAnimation.play(
		getwidth() / 2 - 200,  // 起始X（水平居中）
		getheight() / 2 - 150, // 起始Y（垂直居中）
		getwidth() / 2 - 200,  // 结束X（水平居中）
		getheight() / 2 - 150, // 结束Y（垂直居中）
		[](float progress, int& x, int& y, int sx, int sy, int ex, int ey) {
			PathFunctions::heartbeat(progress, x, y, ex, ey);
		},
		SRCCOPY
	);

	Shuffle(3);
}