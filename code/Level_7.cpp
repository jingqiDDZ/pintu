#include "level.h"

static int value_7 = Level_7::Prob;

void Level_7::initAnimation() {
	display1.init({ L"./assets/image/level/7/all1.png" }, BLOCK_SIZE, BLOCK_SIZE, Animation::NON_BLOCKING, 1000, 100);
	display2.init({ L"./assets/image/level/7/all2.png" }, BLOCK_SIZE, BLOCK_SIZE, Animation::NON_BLOCKING, 1000, 100);
	/*debuffAnimation.init(
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
	);*/
	debuffAnimation.init({ L"./assets/image/level/7/bagua1.png" }, WD_width/3, WD_height/3, Animation::NON_BLOCKING, 1000, 100);



	// 加载动画资源
	if (!display1.loadFrames()) {
		MessageBox(GetHWnd(), _T("图片动画资源加载失败"), _T("错误"), MB_OK);
	}
	if (!display2.loadFrames()) {
		MessageBox(GetHWnd(), _T("图片动画资源加载失败"), _T("错误"), MB_OK);
	}
	if (!debuffAnimation.loadFrames()) {
		MessageBox(GetHWnd(), _T("Debuff动画资源加载失败"), _T("错误"), MB_OK);
	}
	blockImgs1.resize(26); // 0~25
	for (int i = 1; i <= 25; ++i) {
		wchar_t path[256];
		swprintf(path, 256, L"./assets/image/level/7/blocks1/%d.png", i);
		loadimage(&blockImgs1[i], path, BLOCK_SIZE, BLOCK_SIZE);
	}
	blockImgs2.resize(10); // 0~9
	for (int i = 1; i <= 9; ++i) {
		wchar_t path[256];
		swprintf(path, 256, L"./assets/image/level/7/blocks2/%d.png", i);
		loadimage(&blockImgs2[i], path, BLOCK_SIZE, BLOCK_SIZE);
	}
}

void Level_7::Buff_jojo(int n) {
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

void Level_7::Debuff_jojo() {
	kingCrimson.play();

	debuffAnimation.setAlpha(250);
	debuffAnimation.setStayDuration(1000);

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
	/*debuffAnimation.play(
		getwidth(),          // startX
		-300,                // startY
		centerX,             // endX
		centerY,             // endY
		[](float progress, int& x, int& y, int sx, int sy, int ex, int ey) {
			// 使用lambda正确转发参数
			PathFunctions::linear(progress, x, y, sx, sy, ex, ey);
		},  // 正确传递6个参数
		SRCCOPY
	);*/
	debuffAnimation.startNonBlocking(centerX, centerY, centerX, centerY);

	Shuffle(3);
}

void Level_7::Shuffle(int times) {
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

int Level_7::handleFunctionKeys() {
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
		//showWin();

		// 第一阶段胜利后进入第二阶段
		if (stage == 1) {
			stage = 2;
			win_1 = true;
			trans.play();
			// 播放阻塞动画
			//debuffAnimation.play(...); // 你可以自定义动画参数
			// 切换资源
			SSIZE = 3;
			blockImgs = blockImgs2;
			all = all2;
			initBoard();
			shuffleBoard();

		}
		else {
			showWin();
		}


		lastFunctionTime = currentTime;
	}
	else if (GetAsyncKeyState('Q') & 0x8000) {
		if (skillQ == 2) {
			if (history.size() >= 3) {
				Buff_jojo(3);
			}
			else {
				killerQueen_de.play();
			}
			lastFunctionTime = currentTime;
		}
		else if (skillQ == 1) {
			if (stage == 1) {
				printf("Try display1\n");
				if (!display1.isPlaying()) {
					int X = WD_width / 4 * 3;
					int Y = WD_height / 4 * 3;
					display1.setStayDuration(5000);
					display1.startNonBlocking(X, Y, X, Y);
				}
			}
			else if (stage == 2) {
				printf("Try display2\n");
				if (!display2.isPlaying()) {
					int X = WD_width / 4 * 3;
					int Y = WD_height / 4 * 3;
					display2.setStayDuration(5000);
					display2.startNonBlocking(X, Y, X, Y);
				}
			}
			lastFunctionTime = currentTime;
		}
		else if (skillQ == 0) {
			skill0.play();
		}
	}
	else if (GetAsyncKeyState('E') & 0x8000) {
		if (skillE == 2) {
			if (history.size() >= 3) {
				Buff_jojo(3);
			}
			else {
				killerQueen_de.play();
			}
			lastFunctionTime = currentTime;
		}
		else if (skillE == 1) {
			if (stage == 1) {
				printf("Try display1\n");
				if (!display1.isPlaying()) {
					int X = WD_width / 4 * 3;
					int Y = WD_height / 4 * 3;
					display1.setStayDuration(5000);
					display1.startNonBlocking(X, Y, X, Y);
				}
			}
			else if (stage == 2) {
				printf("Try display2\n");
				if (!display2.isPlaying()) {
					int X = WD_width / 4 * 3;
					int Y = WD_height / 4 * 3;
					display2.setStayDuration(5000);
					display2.startNonBlocking(X, Y, X, Y);
				}
			}
			lastFunctionTime = currentTime;
		}
		else if (skillE == 0) {
			skill0.play();
		}
	}
	//回退操作
	/*else if (GetAsyncKeyState('Q') & 0x8000) {
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
		if (stage == 1) {
			printf("Try display1\n");
			if (!display1.isPlaying()) {
				int X = WD_width / 4 * 3;
				int Y = WD_height / 4 * 3;
				display1.setStayDuration(5000);
				display1.startNonBlocking(X, Y, X, Y);
			}
		}
		else if (stage == 2) {
			printf("Try display2\n");
			if (!display2.isPlaying()) {
				int X = WD_width / 4 * 3;
				int Y = WD_height / 4 * 3;
				display2.setStayDuration(5000);
				display2.startNonBlocking(X, Y, X, Y);
			}
		}
		lastFunctionTime = currentTime;
	}*/
}

void Level_7::moveTile(int row, int col) {
	if (stage == 1) {
		Level::moveTile(row, col);
	}
	else {
		if (canMove(row, col)) {

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
			value_7 = (value_7 - stepsize) < Minvalue_Prob ? Minvalue_Prob : (value_7 - stepsize);
			int r = rand() % value_7;
			if (r == 1) {
				Debuff_jojo();
			}
			if (value_7 == Minvalue_Prob) value_7 = Prob;
		}
	}
}

LevelResult Level_7::play() {
	
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
		if (display1.isPlaying() && display1.getType() == Animation::NON_BLOCKING && stage==1) {
			display1.updateNonBlocking();
		}
		if (display2.isPlaying() && display2.getType() == Animation::NON_BLOCKING && stage==2) {
			display2.updateNonBlocking();
		}
		if (debuffAnimation.isPlaying() && debuffAnimation.getType() == Animation::NON_BLOCKING) {
			debuffAnimation.updateNonBlocking();
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
		//	}
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

			// 第一阶段胜利后进入第二阶段
			if (stage == 1) {
				stage = 2;
				win_1 = true;
				trans.play();
				// 播放阻塞动画
				//debuffAnimation.play(...); // 你可以自定义动画参数
				// 切换资源
				SSIZE = 3;
				blockImgs = blockImgs2;
				all = all2;
				initBoard();
				shuffleBoard();
				continue;
			}
			else {
				return LevelResult::Win;
			}

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
				else if (GetAsyncKeyState(VK_ESCAPE) & 0x8008) {
					return LevelResult::Exit;
				}

				// 避免CPU占用过高
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

void Level_7::initBoard() {
	if (stage == 1) {
		win_1 = false;
		SSIZE = 5;
		blockImgs = blockImgs1;
		all = all1;
	}
	else {
		SSIZE = 3;
		blockImgs = blockImgs2;
		all = all2;
		// 初始化透明度
		alphaBoard.assign(SSIZE, vector<BYTE>(SSIZE, 255));
	}
	Level::initBoard();
}

void Level_7::shuffleBoard() {
	if (stage == 1) {
		SSIZE = 5;
	}
	else {
		SSIZE = 3;
	}
	Level::shuffleBoard();
}

void Level_7::drawGame() {
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

	// 绘制计时时间
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
			if (stage == 2) {
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
			}
			else {
				putimage(x, y, &blockImgs[board[i][j]]);//方块用图片填充
			}
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

	// 绘制当前阶段提示
	TCHAR stageText[50];
	_stprintf_s(stageText, _T("当前阶段: %d"), stage);
	outtextxy(SSIZE * 50 + 50, 120, stageText);
}