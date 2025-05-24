#include "game.h"

#define BLOCK_SIZE 100
#define MARGIN  50

int SSIZE;
vector<vector<int>> board;
vector<IMAGE> blockImgs;
int emptyRow, emptyCol;
int moves = 0;

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

	_getch();

	initBoard();
	shuffleBoard();
	sf::Sound sound_bgm;
	sf::SoundBuffer buffer_bgm;
	loadSoundBgm("./assets/audio/bgm_long.wav", sound_bgm, buffer_bgm);

	sf::Sound sound_click;
	sf::SoundBuffer buffer_click;
	loadSoundClip("./assets/audio/click.wav", sound_click, buffer_click);


	bool bgm_start = false;

	while (true) {
		if (!bgm_start) {
			bgm_start = true;
			sound_bgm.play();
		}
		if (handleMouse()) {
			sound_click.play();
		}

		if (isWin()) {
			showWin();
			initBoard();
			shuffleBoard();
			moves = 0;
			continue;
		}

		//处理键盘输入（重启和退出）
		if (_kbhit()) {
			int key = _getch();
			if (key == 'r' || key == 'R') {
				initBoard();
				shuffleBoard();
				moves = 0;
			}
			else if (key == 27) {
				break;
			}
			if (key == 'a') {
				showWin();
				initBoard();
				shuffleBoard();
				moves = 0;
				continue;
			}
		}

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

	TCHAR resultText[50];
	_stprintf_s(resultText, _T("移动次数: %d"), moves);
	settextstyle(30, 0, _T("宋体"));
	outtextxy(SSIZE*50, SSIZE*100+270, resultText);

	settextstyle(20, 0, _T("宋体"));
	outtextxy(150, SSIZE*100+310, _T("按任意键继续"));
	outtextxy(150, SSIZE*100+330, _T("ESC退出游戏"));

	FlushBatchDraw();


	while (true) {
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