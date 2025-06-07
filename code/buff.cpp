#include "level.h"
#include <algorithm>

Animation DebuffAnimation;
Animation display;





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

		drawGame();

		// 更新非阻塞动画
		if (display.isPlaying() && display.getType() == Animation::NON_BLOCKING) {
			display.updateNonBlocking();
		}

		// 检查是否有阻塞动画在播放（如debuffAnimation）
		bool isBlockingAnimation = DebuffAnimation.isPlaying() &&
			(DebuffAnimation.getType() == Animation::BLOCKING);

		// 非动画期间处理输入
		if (!isBlockingAnimation) {
			if (handleMouse() || handleKeyboard()) {
				sound_click.play();
			}

			//处理功能键
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

			return LevelResult::Win;
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
	//检测C键:展示图片
	else if (GetAsyncKeyState('C') & 0x8000) {
		if (!display.isPlaying()) {
			IMAGE all;
			loadimage(&all, _T("./assets/image/level/0/all.png"), BLOCK_SIZE, BLOCK_SIZE);
			int X = WD_width / 4 * 3;
			int Y = WD_height / 4 * 3;
			display.setStayDuration(5000);
			display.startNonBlocking(X, Y, X, Y);
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
	DebuffAnimation.init(
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

	display.init({ L"./assets/image/level/0/all.png" }, BLOCK_SIZE, BLOCK_SIZE, Animation::NON_BLOCKING, 1000, 100);

	//debuffAnimation.init({ L"./assets/anim/te.png" }, 400, 300, Animation::NON_BLOCKING, 1000, 1000);

	// 加载动画资源
	if (!DebuffAnimation.loadFrames()) {
		MessageBox(GetHWnd(), _T("Debuff动画资源加载失败"), _T("错误"), MB_OK);
	}
	if (!display.loadFrames()) {
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

	DebuffAnimation.setAlpha(250);
	DebuffAnimation.setStayDuration(1000);

	/*debuffAnimation.play(
		getwidth() / 2 - 200,  // 起始X（水平居中）
		getheight() / 2 - 150, // 起始Y（垂直居中）
		getwidth() / 2 - 200,  // 结束X（水平居中）
		getheight() / 2 - 150, // 结束Y（垂直居中）
		[](float progress, int& x, int& y, int sx, int sy, int ex, int ey) {
			PathFunctions::heartbeat(progress, x, y, ex, ey);
		},
		SRCCOPY
	);*/

	// 计算屏幕中心位置
	int centerX = getwidth() / 2 - 200;  // 动画宽度400
	int centerY = getheight() / 2 - 150; // 动画高度300

	// 设置起始位置为屏幕右上角外，结束位置为屏幕中心
	DebuffAnimation.play(
		getwidth(),          // 起始X：屏幕右侧外
		-300,                // 起始Y：屏幕上方外
		centerX,             // 结束X：水平居中
		centerY,             // 结束Y：垂直居中
		PathFunctions::linear, // 使用线性移动路径
		SRCCOPY
	);

	Shuffle(3);
}

void Level_TE::initBoard() {
	Level::initBoard();
	// 重置所有透明度为255（完全不透明）
	for (int i = 0; i < SSIZE; i++) {
		for (int j = 0; j < SSIZE; j++) {
			alphaBoard[i][j] = 255;
		}
	}
}

void Level_TE::moveTile(int row, int col) {
	if (canMove(row, col)) {
		// 保存当前状态到历史记录
		history.push_back(board);
		if (history.size() > 50) history.erase(history.begin());

		// 获取被移动方块的值
		int movedValue = board[row][col];

		// 计算该方块的正确位置
		int correctRow = (movedValue - 1) / SSIZE;
		int correctCol = (movedValue - 1) % SSIZE;

		// 检查移动前是否在正确位置
		bool wasInCorrectPosition = (row == correctRow && col == correctCol);

		// 减少被移动块的透明度（无论是否在正确位置）
		if (alphaBoard[row][col] > minAlpha) {
			alphaBoard[row][col] = minAlpha > (alphaBoard[row][col] - alphaDecrement) ? minAlpha : (alphaBoard[row][col] - alphaDecrement);
		}

		// 执行移动
		board[emptyRow][emptyCol] = movedValue;
		board[row][col] = 0;

		// 移动后检查是否在正确位置
		bool nowInCorrectPosition = (emptyRow == correctRow && emptyCol == correctCol);

		// 更新透明度数组 - 透明度应跟随方块移动
		BYTE movedAlpha = alphaBoard[row][col];
		alphaBoard[emptyRow][emptyCol] = nowInCorrectPosition ? 255 : movedAlpha;

		// 如果方块离开正确位置，重置其原始位置的透明度
		if (wasInCorrectPosition && !nowInCorrectPosition) {
			alphaBoard[row][col] = 255; // 重置原始位置的透明度
		}

		// 更新空白位置
		emptyRow = row;
		emptyCol = col;
		moves++;
	}
}

void Level_TE::drawGame() {
	setbkcolor(RGB(240, 240, 240));
	cleardevice();

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
					int idx = y * BLOCK_SIZE + x;
					DWORD color = pSrc[idx];

					BYTE b = GetBValue(color);
					BYTE g = GetGValue(color);
					BYTE r = GetRValue(color);
					BYTE originalAlpha = (color >> 24) & 0xFF;

					// 应用当前透明度
					BYTE finalAlpha = (originalAlpha * alpha) / 255;

					// 设置到目标位图
					pDst[0] = b;
					pDst[1] = g;
					pDst[2] = r;
					pDst[3] = finalAlpha;
					pDst += 4;
				}
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