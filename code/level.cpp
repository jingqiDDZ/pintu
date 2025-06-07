#include "level.h"
#include <thread>


bool Level::gdiplusInitialized = false;
ULONG_PTR Level::gdiplusToken = 0;

Level::Level(int tid, int tSSIZE, int tTmode,int tskillQ,int tskillE) :
	id(tid), SSIZE(tSSIZE), Tmode(tTmode) ,skillQ(tskillQ),skillE(tskillE){

	moves = 0;
	isWinning = false;
	board.resize(SSIZE, vector<int>(SSIZE));		//初始化棋盘数组大小
	blockImgs.resize(SSIZE*SSIZE);		//初始化拼图图片数组大小

	cout << "开始导入游戏素材" << endl;
	//利用id初始化游戏素材路径，并导入
	string audioPath = "./assets/audio/level/" + to_string(id) + "/";		//初始化为"./assets/audio/level/1/"
	string imagePath = "./assets/image/level/" + to_string(id) + "/";

	//音频素材
	string tmppath;
	tmppath = audioPath + "bgm.wav";
	loadSoundBgm(tmppath, sound_bgm, buffer_bgm);
	tmppath = audioPath + "click.wav";
	loadSoundClip(tmppath, sound_click, buffer_click);
	tmppath = audioPath + "win.wav";
	loadSoundClip(tmppath, sound_win, buffer_win);
	loadSoundClip("./assets/audio/skill0.wav", skill0, Buffer_skill0);
	cout << "音频素材导入完毕" << endl;

	//图片素材
	tmppath = imagePath + "start.png";
	wstring wpath(tmppath.begin(), tmppath.end());		//转换字符串类型
	loadimage(&bkImg, wpath.c_str(), 1500, 950);

	for (int i = 0;i < SSIZE * SSIZE;i++) {
		tmppath = imagePath + to_string(i) + ".png";
		wstring wpath(tmppath.begin(), tmppath.end());		//转换字符串类型
		loadimage(&blockImgs[i], wpath.c_str(), BLOCK_SIZE, BLOCK_SIZE);
	}
	cout << "图片素材导入完毕" << endl;

	loadSoundClip("./assets/audio/Buff_jojo.wav", killerQueen, Buffer_killerQueen);
	loadSoundClip("./assets/audio/Buff_jojo_de.wav", killerQueen_de, Buffer_killerQueen_de);

	//初始化华容道总图片
	std::wstring displayPath = L"./assets/image/level/" + std::to_wstring(id) + L"/all.png";
	display.init({ displayPath }, BLOCK_SIZE, BLOCK_SIZE, Animation::NON_BLOCKING, 1000, 100);
	loadimage(&all, displayPath.c_str(), BLOCK_SIZE, BLOCK_SIZE);
	if (!display.loadFrames()) {
		MessageBox(GetHWnd(), _T("图片动画资源加载失败"), _T("错误"), MB_OK);
	}

	//若是倒计时模式则导入倒计时音频，初始化倒计时变量
	if (Tmode == 1) {
		countdownData.isTimeout = false;         // 是否超时(倒计时模式)
		countdownData.Played_count = false;      // 倒计时音乐是否已播放

		tmppath = audioPath + "countdown.wav";
		loadSoundClip(tmppath, countdownData.sound, countdownData.buffer);
	}
	cout << "倒计时素材导入完毕" << endl;
	cout << "关卡" << id << "初始化完毕" << endl;
}

LevelResult Level::play() {
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

		if (handleMouse() || handleKeyboard()) {
			sound_click.play();
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

			//结算之后，展示对话剧情再结束游戏
			string tmppath = "./assets/text/level/" + to_string(id) + "/dialogue2.json";
			drawDialogue(tmppath);
			FlushBatchDraw();
			//赢了之后直接return Win
			return LevelResult::Win;
			//continue;
		}

		//处理功能键		这里还要改，推出窗口之类的问题
		int funcReturn = handleFunctionKeys();
		if (funcReturn == 1) {
			return LevelResult::Exit;
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


//绘制游戏菜单
void Level::drawGame() {
	setbkcolor(RGB(240, 240, 240));
	cleardevice();

	//绘制背景
	//putimage_alpha(0,0,&bkImg);

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

	// 绘制游戏方块
	for (int i = 0; i < SSIZE; i++) {
		for (int j = 0; j < SSIZE; j++) {
			int x = MARGIN + j * BLOCK_SIZE;
			int y = MARGIN + i * BLOCK_SIZE + 80;

			putimage(x, y, &blockImgs[board[i][j]]);//方块用图片填充

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

void Level::drawDialogue(string path) {
	//加载此处的对话数据
	cout << "加载对话" << path << endl;
	delete dialogue;
	cout << "加载对话" << path << endl;
	dialogue = new Dialogue(path);
	cout << "加载对话" << path << endl;
	dialogue->draw();
}


//显示游戏界面背景，我感觉这里可以加过场动画社么的，先保留了
void Level::Gameopen() {
	fadeInImage(&bkImg, 0, 0, 1000);
	FlushBatchDraw();
}


//初始化棋盘（拼图板） copy
void Level::initBoard() {
	int num = 1;
	for (int i = 0;i < SSIZE;i++) {
		for (int j = 0;j < SSIZE;j++) {
			board[i][j] = num++;
		}
	}
	emptyRow = SSIZE - 1;
	emptyCol = SSIZE - 1;
	board[emptyRow][emptyCol] = 0;
	history.clear();
}

//打乱棋盘（拼图板） copy
void Level::shuffleBoard() {
	srand((unsigned)time(NULL));
	initBoard();  // 初始化为已解决状态
	int preDir[1][2];//先前移动的反方向
	int last = -1;//初始没有先前移动方向
	// 预生成所有可能的合法移动对
	for (int i = 0; i < 200; i++) {
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
			// 执行交换（模拟移动）
			board[emptyRow][emptyCol] = board[targetRow][targetCol];
			board[targetRow][targetCol] = 0;
			emptyRow = targetRow;
			emptyCol = targetCol;
		}
	}
	moves = 0;
	history.clear();
}

//判断是否胜利 copy
bool Level::isWin() {
	int num = 1;
	for (int i = 0; i < SSIZE; i++) {
		for (int j = 0; j < SSIZE; j++) {
			if (i == SSIZE - 1 && j == SSIZE - 1) {
				if (board[i][j] != 0) return false;
			}
			else {
				if (board[i][j] != num++) return false;
			}
		}
	}
	return true;
}

//胜利结算 copy
void Level::showWin() {
	//setfillcolor(RGB(0, 0, 0));
	//solidrectangle(0, 0, getwidth(), getheight());
	drawGame(); // 先绘制游戏界面
	FlushBatchDraw();

	HDC hdc = GetImageHDC(); // 获取 EasyX 的 HDC

	// 创建半透明画刷
	Graphics graphics(hdc);
	Gdiplus::Color color(128, 0, 0, 0); // ARGB(透明度, R, G, B)
	SolidBrush brush(color);

	// 覆盖整个窗口
	graphics.FillRectangle(&brush, 0, 0, getwidth(), getheight());

	settextcolor(BLACK);
	settextstyle(40, 0, _T("宋体"));
	outtextxy(SSIZE * 50 - 20, SSIZE * 100 + 230, _T("恭喜你赢了!"));

	TCHAR resultText[100];
	if (Tmode == 0) {
		int total = difftime(timeData.endTime, timeData.startTime);
		int minutes = total / 60;
		int seconds = total % 60;
		_stprintf_s(resultText, _T("移动次数: %d  用时: %02d:%02d"), moves, minutes, seconds);
	}
	else  if (Tmode == 1) {
		_stprintf_s(resultText, _T("移动次数: %d  剩余时间: %02d:%02d"),
			moves,
			static_cast<int>(countdownData.totalTime - countdownData.used.count()) / 60,
			static_cast<int>(countdownData.totalTime - countdownData.used.count()) % 60);
	}
	settextstyle(30, 0, _T("宋体"));
	outtextxy(SSIZE * 50 - 20, SSIZE * 100 + 270, resultText);


	settextstyle(20, 0, _T("宋体"));
	outtextxy(150, SSIZE * 100 + 310, _T("按任意键继续"));
	outtextxy(150, SSIZE * 100 + 330, _T("ESC退出游戏"));
	FlushBatchDraw();


	sound_bgm.pause();//暂停背景音乐
	sound_win.play();//播放胜利音乐

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
			//closegraph();			//这里也要改
			//exit(0);
			return;
		}
		// 检查任意键
		for (int vKey = 8; vKey <= 255; vKey++) {
			if (GetAsyncKeyState(vKey) & 0x8000) {
				//goto CONTINUE_GAME;
				return;
			}
		}
		Sleep(30);
	}
/*
CONTINUE_GAME:

	sound_bgm.play();
	sound_win.pause();


	//按任意键重置棋盘
	initBoard();
	shuffleBoard();
	moves = 0;
	countdownData.Played_count = false;

	if (Tmode == 0) {
		isWinning = false;
		time(&timeData.startTime);
	}
	else if (Tmode == 1) {
		countdownData.startTime = std::chrono::system_clock::now();
		countdownData.isTimeout = false;
	}

*/
}


//移动逻辑
//判断是否可以移动
bool Level::canMove(int row, int col) {
	return (abs(row - emptyRow) == 1 && col == emptyCol) ||
		(abs(col - emptyCol) == 1 && row == emptyRow);
}

//move
void Level::moveTile(int row, int col) {
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
	}
}

//按键检测函数
//鼠标点击处理 copy
bool Level::handleMouse() {
	MOUSEMSG msg;
	if (MouseHit()) {  // 检查是否有鼠标消息
		msg = GetMouseMsg();

		if (msg.uMsg == WM_LBUTTONDOWN) {  // 左键按下
			// 计算点击的方块坐标
			int col = (msg.x - MARGIN) / BLOCK_SIZE;
			int row = (msg.y - MARGIN - 80) / BLOCK_SIZE;

			// 检查点击是否在有效范围内
			if (row >= 0 && row < SSIZE && col >= 0 && col < SSIZE) {
				moveTile(row, col);
				return true;
			}
		}
	}
	return false;
}

//键盘移动处理 copy
bool Level::handleKeyboard() {
	DWORD currentTime = GetTickCount();	// 如果距离上次移动时间太短，则不处理
	if (currentTime - lastMoveTime < moveDelay) {
		return false;
	}

	int targetRow = emptyRow;
	int targetCol = emptyCol;
	bool moved = false;

	// 检测 WASD 和方向键
	if (GetAsyncKeyState('W') & 0x8000 || GetAsyncKeyState(VK_UP) & 0x8000) {
		targetRow--;
		moved = true;
	}
	else if (GetAsyncKeyState('S') & 0x8000 || GetAsyncKeyState(VK_DOWN) & 0x8000) {
		targetRow++;
		moved = true;
	}
	else if (GetAsyncKeyState('A') & 0x8000 || GetAsyncKeyState(VK_LEFT) & 0x8000) {
		targetCol--;
		moved = true;
	}
	else if (GetAsyncKeyState('D') & 0x8000 || GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		targetCol++;
		moved = true;
	}

	if (moved && targetRow >= 0 && targetRow < SSIZE && targetCol >= 0 && targetCol < SSIZE) {
		moveTile(targetRow, targetCol);
		lastMoveTime = currentTime;
		return true;
	}
	return false;
}

//功能键处理 jojo注释掉了，修改返回值，如果退出的话play函数要返回值
int Level::handleFunctionKeys() {
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
		//结算之后，展示对话剧情再结束游戏
		string tmppath = "./assets/text/level/" + to_string(id) + "/dialogue2.json";
		drawDialogue(tmppath);
		FlushBatchDraw();
		//赢了之后直接return Win
		//return LevelResult::Win;

		lastFunctionTime = currentTime;
	}
	else if (GetAsyncKeyState('Q') & 0x8000) {
		if (skillQ == 1) {
			if (!display.isPlaying()) {
				int X = WD_width / 4 * 3;
				int Y = WD_height / 4 * 3;
				display.setStayDuration(5000);
				display.startNonBlocking(X, Y, X, Y);
			}
			lastFunctionTime = currentTime;
		}
		else if (skillQ == 2) {
			if (history.size() >= 3) {
				Buff_jojo(3);
			}
			else {
				killerQueen_de.play();
			}
			lastFunctionTime = currentTime;
		}
		else if (skillQ == 0) {
			skill0.play();
		}
	}
	else if (GetAsyncKeyState('E') & 0x8000) {
		if (skillE == 1) {
			if (!display.isPlaying()) {
				int X = WD_width / 4 * 3;
				int Y = WD_height / 4 * 3;
				display.setStayDuration(5000);
				display.startNonBlocking(X, Y, X, Y);
			}
			lastFunctionTime = currentTime;
		}
		else if (skillE == 2) {
			if (history.size() >= 3) {
				Buff_jojo(3);
			}
			else {
				killerQueen_de.play();
			}
			lastFunctionTime = currentTime;
		}
		else if (skillE == 0) {
			skill0.play();
		}
	}
}

void Level::fadeImage(IMAGE* img, int x, int y, int fadeInTime, int stayTime, int fadeOutTime) {
	// 初始化GDI+（如果尚未初始化）
	static bool gdiplusInitialized = false;
	static ULONG_PTR gdiplusToken;
	if (!gdiplusInitialized) {
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		gdiplusInitialized = true;
	}

	// 获取当前窗口的HDC
	HDC hdc = GetImageHDC();
	Graphics graphics(hdc);

	// 将EasyX的IMAGE转换为GDI+的Bitmap
	int width = img->getwidth();
	int height = img->getheight();
	Bitmap bmp(width, height, PixelFormat32bppARGB);
	BitmapData bmpData;
	Gdiplus::Rect rect(0, 0, width, height);
	bmp.LockBits(&rect, ImageLockModeWrite, PixelFormat32bppARGB, &bmpData);

	// 复制图像数据
	DWORD* src = (DWORD*)GetImageBuffer(img);
	DWORD* dst = (DWORD*)bmpData.Scan0;
	int pitch = bmpData.Stride / 4; // 每行DWORD数

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			dst[i * pitch + j] = src[i * width + j];
		}
	}
	bmp.UnlockBits(&bmpData);

	// 淡入效果
	for (int alpha = 0; alpha <= 255; alpha += 5) {
		cleardevice();

		// 设置透明度
		ColorMatrix matrix = {
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, alpha / 255.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f
		};

		ImageAttributes attr;
		attr.SetColorMatrix(&matrix);

		// 绘制图像
		graphics.DrawImage(
			&bmp,
			Gdiplus::Rect(x, y, width, height),
			0, 0, width, height,
			UnitPixel, &attr
		);

		FlushBatchDraw();
		Sleep(fadeInTime / 50); // 控制淡入速度
	}

	// 停留效果
	Sleep(stayTime);

	// 淡出效果
	for (int alpha = 255; alpha >= 0; alpha -= 5) {
		cleardevice();

		// 设置透明度
		ColorMatrix matrix = {
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, alpha / 255.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f
		};

		ImageAttributes attr;
		attr.SetColorMatrix(&matrix);

		// 绘制图像
		graphics.DrawImage(
			&bmp,
			Gdiplus::Rect(x, y, width, height),
			0, 0, width, height,
			UnitPixel, &attr
		);

		FlushBatchDraw();
		Sleep(fadeOutTime / 50); // 控制淡出速度
	}
}


void Level::fadeInImage(IMAGE* img, int x, int y, int fadeInTime) {
	// 初始化GDI+（如果尚未初始化）
	static bool gdiplusInitialized = false;
	static ULONG_PTR gdiplusToken;
	if (!gdiplusInitialized) {
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		gdiplusInitialized = true;
	}

	// 获取当前窗口的HDC
	HDC hdc = GetImageHDC();
	Graphics graphics(hdc);

	// 将EasyX的IMAGE转换为GDI+的Bitmap
	int width = img->getwidth();
	int height = img->getheight();
	Bitmap bmp(width, height, PixelFormat32bppARGB);
	BitmapData bmpData;
	Gdiplus::Rect rect(0, 0, width, height);
	bmp.LockBits(&rect, ImageLockModeWrite, PixelFormat32bppARGB, &bmpData);

	// 复制图像数据
	DWORD* src = (DWORD*)GetImageBuffer(img);
	DWORD* dst = (DWORD*)bmpData.Scan0;
	int pitch = bmpData.Stride / 4; // 每行DWORD数

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			dst[i * pitch + j] = src[i * width + j];
		}
	}
	bmp.UnlockBits(&bmpData);

	// 淡入效果
	for (int alpha = 0; alpha <= 255; alpha += 5) {
		cleardevice();

		// 设置透明度
		ColorMatrix matrix = {
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, alpha / 255.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f
		};

		ImageAttributes attr;
		attr.SetColorMatrix(&matrix);

		// 绘制图像
		graphics.DrawImage(
			&bmp,
			Gdiplus::Rect(x, y, width, height),
			0, 0, width, height,
			UnitPixel, &attr
		);

		FlushBatchDraw();
		Sleep(fadeInTime / 50); // 控制淡入速度
	}
}

void Level::fadeOutImage(IMAGE* img, int x, int y, int fadeOutTime) {
	// 初始化GDI+（如果尚未初始化）
	static bool gdiplusInitialized = false;
	static ULONG_PTR gdiplusToken;
	if (!gdiplusInitialized) {
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		gdiplusInitialized = true;
	}

	// 获取当前窗口的HDC
	HDC hdc = GetImageHDC();
	Graphics graphics(hdc);

	// 将EasyX的IMAGE转换为GDI+的Bitmap
	int width = img->getwidth();
	int height = img->getheight();
	Bitmap bmp(width, height, PixelFormat32bppARGB);
	BitmapData bmpData;
	Gdiplus::Rect rect(0, 0, width, height);
	bmp.LockBits(&rect, ImageLockModeWrite, PixelFormat32bppARGB, &bmpData);

	// 复制图像数据
	DWORD* src = (DWORD*)GetImageBuffer(img);
	DWORD* dst = (DWORD*)bmpData.Scan0;
	int pitch = bmpData.Stride / 4; // 每行DWORD数

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			dst[i * pitch + j] = src[i * width + j];
		}
	}
	bmp.UnlockBits(&bmpData);

	// 淡出效果
	for (int alpha = 255; alpha >= 0; alpha -= 5) {
		cleardevice();

		// 设置透明度
		ColorMatrix matrix = {
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, alpha / 255.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f
		};

		ImageAttributes attr;
		attr.SetColorMatrix(&matrix);

		// 绘制图像
		graphics.DrawImage(
			&bmp,
			Gdiplus::Rect(x, y, width, height),
			0, 0, width, height,
			UnitPixel, &attr
		);

		FlushBatchDraw();
		Sleep(fadeOutTime / 50); // 控制淡出速度
	}
}

void Level::Buff_jojo(int n) {
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

