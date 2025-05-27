#include "game.h"

#define BLOCK_SIZE 100
#define MARGIN  50

time_t startTime, endTime;//计时时间
bool isWinning = false;//计时的胜利判定用的
int SSIZE;
vector<vector<int>> board;
vector<IMAGE> blockImgs;
int emptyRow, emptyCol;
int moves = 0;
int totalTime = 15;//倒计时总时间
int remaining;
chrono::time_point<chrono::system_clock>startTime_Re, currentTime_Re;
chrono::duration<double> used;
bool  isTimeout = false;//倒计时胜利判定的
bool Played_count = false;//倒计时音乐是否播放过了
int Tmode = 1;//0表示计时模式，1表示倒计时模式


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
void Gameopen();//游戏启动界面
void Debuff_jojo(sf::Sound& kingCrimson);//红王debuff，随机移动两步

int main() {
	SSIZE = 3;
	board.resize(SSIZE, vector<int>(SSIZE));
	blockImgs.resize(SSIZE * SSIZE);

	initgraph(SSIZE * 100 + 200, SSIZE * 100 + 400);//游戏板大小
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

	_getch();

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
	sf::Sound sound_bgm;
	sf::SoundBuffer buffer_bgm;
	loadSoundBgm("./assets/audio/bgm_long.wav", sound_bgm, buffer_bgm);

	sf::Sound sound_click;
	sf::SoundBuffer buffer_click;
	loadSoundClip("./assets/audio/click.wav", sound_click, buffer_click);

	//后期位置可能变化
	sf::Sound kingCrimson;
	sf::SoundBuffer buffer_kingCrimson;
	loadSoundClip("./assets/audio/Debuff_jojo.wav", kingCrimson, buffer_kingCrimson);
	//标记一下

	//倒计时音乐
	sf::Sound sound_Timeout;
	sf::SoundBuffer buffer_Timeout;
	loadSoundClip("./assets/audio/Countdown.wav", sound_Timeout, buffer_Timeout);
	//标记

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

			if (remaining <= 10&&Played_count==false) {
				sound_Timeout.play();
				Played_count = true;
			}
		}//倒计时

		if (!bgm_start) {
			bgm_start = true;
			sound_bgm.play();
		}
		if (handleMouse()) {
			sound_click.play();
		}

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
			cout << "showWin finished" << endl;

			initBoard();
			shuffleBoard();
			moves = 0;
			Played_count = false;
			if (Tmode == 0) {
				isWinning = false;
				time(&startTime);
			}
			continue;
		}

		//处理键盘输入（重启和退出）
		if (_kbhit()) {
			int key = _getch();
			if (key == 'r' || key == 'R') {
				initBoard();
				shuffleBoard();
				moves = 0;
				if (Tmode == 0) {
					time(&startTime);  // 重置开始时间
					isWinning = false;
				}
				else if (Tmode == 1) {
					startTime_Re = std::chrono::system_clock::now(); // 重置计时器
					isTimeout = false;
				}
			}
			else if (key == 27) {
				break;
			}
			if (key == 'a') {
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

				initBoard();
				shuffleBoard();
				moves = 0;
				Played_count = false;
				if (Tmode == 0) {
					isWinning = false;
					time(&startTime);
				}
				continue;
			}
			if (key == 'b') {
				Debuff_jojo(kingCrimson);
			}
		}

		if (isTimeout&&Tmode==1) {
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

			// 处理输入
			if (_kbhit()) {
				int key = _getch();
				if (key == 'r' || key == 'R') {
					initBoard();
					shuffleBoard();
					moves = 0;
					Played_count = false;
					startTime_Re = std::chrono::system_clock::now();
					isTimeout = false;
				}
				else if (key == 27) {
					break;
				}
			}
			continue; // 跳过后续游戏逻辑
		}//倒计时超时

		drawGame();
		FlushBatchDraw();
		Sleep(0.1);
	}

	closegraph();
	return 0;
}

void Gameopen() {
	IMAGE Start;
	loadimage(&Start, _T("./assets/image/Start.png"),getwidth(),getheight());//载入开始画面

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
}

bool canMove(int row, int col) {
	return (abs(row - emptyRow) == 1 && col == emptyCol) ||
		(abs(col - emptyCol) == 1 && row == emptyRow);
}

void moveTile(int row, int col) {
	if (canMove(row, col)) {
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
	Color color(128, 0, 0, 0); // ARGB(透明度, R, G, B)
	SolidBrush brush(color);

	// 覆盖整个窗口
	graphics.FillRectangle(&brush, 0, 0, getwidth(), getheight());

	settextcolor(BLACK);
	settextstyle(40, 0, _T("宋体"));
	outtextxy(SSIZE*50-20, SSIZE*100+230, _T("恭喜你赢了!"));

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


	while (true) {
		//cout << "kk" << endl;

		if (_kbhit()) {
			int key = _getch();
			if (key == 27) exit(0);
			break;
		}
		Sleep(100);
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
			// 执行交换（模拟移动）
			board[emptyRow][emptyCol] = board[targetRow][targetCol];
			board[targetRow][targetCol] = 0;
			emptyRow = targetRow;
			emptyCol = targetCol;
			moves++;
		}
	}
}

void Debuff_jojo(sf::Sound& kingCrimson) {
	int mark = 0;
	//kingCrimson.setVolume(200);
	kingCrimson.play();
	Shuffle(3);
	FlushBatchDraw();
	Sleep(1000);//听bgm;
}