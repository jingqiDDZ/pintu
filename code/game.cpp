#include "game.h"
#include "Animation.h"
//我是亚龙人

#define BLOCK_SIZE 100
#define MARGIN  50

time_t startTime, endTime;//计时时间
bool isWinning = false;//计时的胜利判定用的
int SSIZE;
vector<vector<int>> board;
vector<vector<vector<int>>> history; // 操作历史记录
vector<IMAGE> blockImgs;
int emptyRow, emptyCol;
int moves = 0;
int backTracking = 3;//回溯次数
int totalTime = 300;//倒计时总时间
int remaining;
chrono::time_point<chrono::system_clock>startTime_Re, currentTime_Re;
chrono::duration<double> used;
bool  isTimeout = false;//倒计时胜利判定的
bool Played_count = false;//倒计时音乐是否播放过了
int Tmode = 1;//0表示计时模式，1表示倒计时模式
DWORD lastMoveTime = 0;       // 移动键最后触发时间
DWORD lastFunctionTime = 0;   // 功能键最后触发时间
const DWORD moveDelay = 150;  // 移动键冷却时间（毫秒）
const DWORD functionDelay = 300; // 功能键冷却时间（毫秒）


Animation debuffAnimation;


void Shuffle(int times);//根据次数打乱棋盘(算步数的)
void loadImages();//加载方块图片资源
void initBoard();//初始化游戏板
void shuffleBoard();//打乱游戏板
bool canMove(int row, int  col);//检查点击方块是否可以移动
void moveTile(int row, int col);//处理方块移动
void drawGame();//绘制游戏界面
bool isWin();//胜利判定
void showWin();//胜利结算
bool handleMouse();//鼠标点击处理
bool handleKeyboard();//键盘移动处理
void Gameopen();//游戏启动界面
void handleFunctionKeys();//功能键处理函数
void Debuff_jojo();//红王debuff，随机移动两步
void Buff_jojo(int n);//败者食尘buff，回退n步
void initAnimations();// 声明动画初始化函数



Sound sound_bgm;//背景音乐
SoundBuffer buffer_bgm;
Sound sound_click;//点击音效
SoundBuffer buffer_click;
Sound kingCrimson;//debuff_jojo音乐
SoundBuffer buffer_kingCrimson;
Sound killerQueen;//buff_jojo音乐
SoundBuffer buffer_killerQueen;
Sound killerQueen_de;//buff_jojo发动失败音乐
SoundBuffer buffer_killerQueen_de;
Sound sound_Timeout;//倒计时音乐
SoundBuffer buffer_Timeout;
Sound sound_win;//胜利音乐
SoundBuffer buffer_win;




int main() {
	SSIZE = 3;
	board.resize(SSIZE, vector<int>(SSIZE));
	blockImgs.resize(SSIZE * SSIZE);


	initgraph(SSIZE * 100 + 400, SSIZE * 100 + 600);//游戏板大小
	loadImages();
	BeginBatchDraw();//双缓冲防止游戏闪烁
	Gameopen();
	/*SetWindowText(GetHWnd(), _T("数字华容道"));
	loadImages();
	BeginBatchDraw();//双缓冲防止游戏闪烁

	setbkcolor(RGB(240, 240, 240));
	cleardevice();

	settextcolor(BLACK);
	settextstyle(30, 0, _T("宋体"));
	outtextxy(120, 150, _T("数字华容道"));

	settextstyle(20, 0, _T("宋体"));
	outtextxy(120, 200, _T("按任意键开始"));
	FlushBatchDraw();*/

	FlushBatchDraw();  // 刷新缓冲区，显示初始界面


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
			closegraph();
			exit(0);
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

	initBoard();
	shuffleBoard();
	if (Tmode == 0) {
		time(&startTime);
		isWinning = false;
	}
	else if (Tmode == 1) {
		startTime_Re = chrono::system_clock::now();
		isTimeout = false;
	}

	loadSoundBgm("./assets/audio/S.T.A.Y.wav", sound_bgm, buffer_bgm);

	loadSoundClip("./assets/audio/click.wav", sound_click, buffer_click);

	loadSoundClip("./assets/audio/Debuff_jojo.wav", kingCrimson, buffer_kingCrimson);

	loadSoundClip("./assets/audio/Buff_jojo.wav", killerQueen, buffer_killerQueen);//发动败者食尘成功的音乐

	loadSoundClip("./assets/audio/Buff_jojo_de.wav", killerQueen_de, buffer_killerQueen_de);//发动败者食尘失败的音乐

	loadSoundClip("./assets/audio/Countdown.wav", sound_Timeout, buffer_Timeout);//倒计时音乐

	loadSoundBgm("./assets/audio/sound_win.wav", sound_win, buffer_win);//胜利音乐

	initAnimations();

	bool bgm_start = false;

	while (true) {

		if (Tmode == 1) {
			auto now = std::chrono::system_clock::now();
			std::chrono::duration<double> elapsed_Re = now - startTime_Re;
			remaining = totalTime - static_cast<int>(elapsed_Re.count());

			if (remaining <= 0 && !isWin()) {
				isTimeout = true;
				remaining = 0;
			}

			if (remaining <= 10 && Played_count == false) {
				sound_Timeout.play();
				Played_count = true;
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

			// 处理功能键（包括Debuff按键）
			handleFunctionKeys();
		}

		//胜利检测
		if (isWin()) {
			if (Tmode == 0) {
				time(&endTime);
				isWinning = true;
			}
			else  if (Tmode == 1) {
				// 计算用时
				auto endTime_Re = std::chrono::system_clock::now();
				used = endTime_Re - startTime_Re;
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
		if (isTimeout && Tmode == 1) {
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
					Played_count = false;
					startTime_Re = std::chrono::system_clock::now();
					isTimeout = false;
					break;
				}
				// 检测ESC键
				else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
					closegraph();
					exit(0);
				}

				// 避免CPU占用过高
				Sleep(50);
			}
			continue; // 跳过后续游戏逻辑
		}//倒计时超时


		drawGame();
		FlushBatchDraw();
		Sleep(0.1);
	}//游戏运行

	closegraph();
	return 0;
}

void Gameopen() {
	IMAGE Start;
	loadimage(&Start, _T("./assets/image/Start.png"), getwidth(), getheight());//载入开始画面

	putimage(0, 0, &Start);

	FlushBatchDraw();
}

void loadImages() {
	for (int i = 0; i <= SSIZE * SSIZE - 1; i++) {
		TCHAR path[256];
		//"./assets/image/0.bmp"

		_stprintf_s(path, _T("./assets/image/%d.png"), i); // 图片路径为img文件夹下的0.bmp到8.bmp  
		loadimage(&blockImgs[i], path, BLOCK_SIZE, BLOCK_SIZE);
		if (blockImgs[i].getwidth() == 0 || blockImgs[i].getheight() == 0) {
			MessageBox(GetHWnd(), path, _T("图片加载失败"), MB_OK);
			exit(1);
		}
	}
}

void initBoard() {
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

void shuffleBoard() {
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

bool canMove(int row, int col) {
	return (abs(row - emptyRow) == 1 && col == emptyCol) ||
		(abs(col - emptyCol) == 1 && row == emptyRow);
}

void moveTile(int row, int col) {
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

void drawGame() {
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
			currentTime = endTime;
		}
		else {
			time(&currentTime);
		}
		int elapsed = difftime(currentTime, startTime);
		int minutes = elapsed / 60;
		int seconds = elapsed % 60;

		TCHAR timeText[50];
		_stprintf_s(timeText, _T("时间: %02d:%02d"), minutes, seconds);
		outtextxy(SSIZE * 50 + 50, 80, timeText);  // 调整位置避免重叠
	}
	else if (Tmode == 1) {
		int minutes = remaining / 60;
		int seconds = remaining % 60;

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

bool isWin() {
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

void showWin() {
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
		int total = difftime(endTime, startTime);
		int minutes = total / 60;
		int seconds = total % 60;
		_stprintf_s(resultText, _T("移动次数: %d  用时: %02d:%02d"), moves, minutes, seconds);
	}
	else  if (Tmode == 1) {
		_stprintf_s(resultText, _T("移动次数: %d  剩余时间: %02d:%02d"),
			moves,
			static_cast<int>(totalTime - used.count()) / 60,
			static_cast<int>(totalTime - used.count()) % 60);
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
			closegraph();
			exit(0);
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

	sound_bgm.play();
	sound_win.pause();


	//按任意键重置棋盘
	initBoard();
	shuffleBoard();
	moves = 0;
	Played_count = false;

	if (Tmode == 0) {
		isWinning = false;
		time(&startTime);
	}
	else if (Tmode == 1) {
		startTime_Re = std::chrono::system_clock::now();
		isTimeout = false;
	}
}

bool handleMouse() {
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


bool handleKeyboard() {
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

void Shuffle(int times) {
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

void Debuff_jojo() {
	int mark = 0;
	kingCrimson.play();

	debuffAnimation.setAlpha(200);

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
	//drawGame();
	//FlushBatchDraw();
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

void handleFunctionKeys() {
	DWORD currentTime = GetTickCount();

	// 检查是否在冷却时间内
	if (currentTime - lastFunctionTime < functionDelay) {
		return;
	}

	// 检测 R 键：重启游戏
	if (GetAsyncKeyState('R') & 0x8000) {
		initBoard();
		shuffleBoard();
		moves = 0;
		if (Tmode == 0) {
			time(&startTime);
			isWinning = false;
		}
		else if (Tmode == 1) {
			startTime_Re = std::chrono::system_clock::now();
			isTimeout = false;
		}
		lastFunctionTime = currentTime;
	}
	// 检测 ESC 键：退出游戏
	else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
		exit(0);
	}
	// 检测 X 键：随机移动
	else if (GetAsyncKeyState('X') & 0x8000) {
		Debuff_jojo();
		lastFunctionTime = currentTime;
	}
	// 检测 V 键：回退操作
	else if (GetAsyncKeyState('V') & 0x8000) {
		if (history.size() >= backTracking) {
			Buff_jojo(3);
		}
		else {
			killerQueen_de.play();
		}
		lastFunctionTime = currentTime;
	}
	// 检测 Z 键：测试胜利
	else if (GetAsyncKeyState('Z') & 0x8000) {
		if (Tmode == 0) {
			time(&endTime);
			isWinning = true;
		}
		else  if (Tmode == 1) {
			// 计算用时
			auto endTime_Re = std::chrono::system_clock::now();
			used = endTime_Re - startTime_Re;
		}
		cout << "WOW~~ isWin!" << endl;
		showWin();
		cout << "showWin finished" << endl;


		lastFunctionTime = currentTime;
	}
}


// 初始化所有动画
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
