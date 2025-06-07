#include "level.h"
#include <algorithm>

Animation DebuffAnimation;
Animation display;





LevelResult Level_TE::play() {
	//SSIZE = 3;
	//board.resize(SSIZE, vector<int>(SSIZE));
	//blockImgs.resize(SSIZE * SSIZE);
	//initgraph(SSIZE * 100 + 400, SSIZE * 100 + 600);//��Ϸ���С		��һ������ע�ͣ�������´���

	BeginBatchDraw();	//˫�����ֹ��Ϸ��˸
	Gameopen();			//���ƿ�ʼ����
	FlushBatchDraw();   //ˢ�»���������ʾ��ʼ����

	// ����1���ȴ����а����ɿ�
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

	// ����2���ȴ���Ұ��������
	while (true) {
		// ���ESC
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
			return LevelResult::Exit;
		}
		// ��������
		for (int vKey = 8; vKey <= 255; vKey++) {
			if (GetAsyncKeyState(vKey) & 0x8000) {
				//չʾ�Ի�����֮���ٿ�ʼ��Ϸ
				string tmppath = "./assets/text/level/" + to_string(id) + "/dialogue.json";
				drawDialogue(tmppath);
				FlushBatchDraw();
				goto CONTINUE_GAME;
			}
		}
		Sleep(30);
	}

CONTINUE_GAME:

	//�������������Ϸ
	//initBoard();
	shuffleBoard();
	if (Tmode == 0) {
		time(&timeData.startTime);
	}
	else if (Tmode == 1) {
		countdownData.startTime = chrono::system_clock::now();
	}

	loadSoundClip("./assets/audio/Debuff_jojo.wav", kingCrimson, Buffer_kingCrimson);

	loadSoundClip("./assets/audio/Buff_jojo.wav", killerQueen, Buffer_killerQueen);//��������ʳ���ɹ�������

	loadSoundClip("./assets/audio/Buff_jojo_de.wav", killerQueen_de, Buffer_killerQueen_de);//��������ʳ��ʧ�ܵ�����



	initAnimations();
	bool bgm_start = false;			//�����һ�ν���ѭ��ʱ��ʼ����bgm



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
		}//����ʱ

		if (!bgm_start) {
			bgm_start = true;
			sound_bgm.play();
		}

		drawGame();

		// ���·���������
		if (display.isPlaying() && display.getType() == Animation::NON_BLOCKING) {
			display.updateNonBlocking();
		}

		// ����Ƿ������������ڲ��ţ���debuffAnimation��
		bool isBlockingAnimation = DebuffAnimation.isPlaying() &&
			(DebuffAnimation.getType() == Animation::BLOCKING);

		// �Ƕ����ڼ䴦������
		if (!isBlockingAnimation) {
			if (handleMouse() || handleKeyboard()) {
				sound_click.play();
			}

			//�����ܼ�
			int funcReturn = handleFunctionKeys();
			if (funcReturn == 1) {
				return LevelResult::Exit;
			}
		}

		//ʤ�����
		if (isWin()) {
			if (Tmode == 0) {
				time(&timeData.endTime);
				isWinning = true;
			}
			else  if (Tmode == 1) {
				// ������ʱ
				auto endTime_Re = chrono::system_clock::now();
				countdownData.used = endTime_Re - countdownData.startTime;
			}
			cout << "WOW~~ isWin!" << endl;
			showWin();

			return LevelResult::Win;
		}

		if (isBlockingAnimation) {
			// �����������Լ��Ļ����߼��������������
			continue;
		}

		//��ʱ���
		if (Tmode == 1 && countdownData.isTimeout) {
			// ���Ƴ�ʱ����
			setbkcolor(RGB(240, 240, 240));
			cleardevice();

			settextcolor(RED);
			settextstyle(40, 0, _T("����"));
			outtextxy(SSIZE * 50 - 40, SSIZE * 100 + 230, _T("ʱ��ľ�!"));

			settextstyle(20, 0, _T("����"));
			outtextxy(150, SSIZE * 100 + 270, _T("��R���¿�ʼ"));
			outtextxy(150, SSIZE * 100 + 300, _T("ESC�˳���Ϸ"));

			FlushBatchDraw();

			// �������������
			while (true) {
				// ���R��
				if (GetAsyncKeyState('R') & 0x8000) {
					initBoard();
					shuffleBoard();
					moves = 0;
					countdownData.Played_count = false;
					countdownData.startTime = chrono::system_clock::now();
					countdownData.isTimeout = false;
					break;
				}
				// ���ESC��
				else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
					return LevelResult::Exit;
				}

				// ����CPUռ�ù���		ţ�ƣ����������֣�����̫����~
				Sleep(50);
			}
			continue; // ����������Ϸ�߼�
		}//����ʱ��ʱ

		FlushBatchDraw();
		Sleep(0.1);
	}//��Ϸ����

	//closegraph();		��Ϸ�������رմ���
	return LevelResult::Exit;
}

int Level_TE::handleFunctionKeys() {
	DWORD currentTime = GetTickCount();

	// ����Ƿ�����ȴʱ����
	if (currentTime - lastFunctionTime < functionDelay) {
		return 0;
	}

	// ��� R ����������Ϸ
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
	// ��� ESC �����˳���Ϸ
	else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
		return 1;
	}

	// ��� Z ��������ʤ��
	else if (GetAsyncKeyState('Z') & 0x8000) {
		if (Tmode == 0) {
			time(&timeData.endTime);
			isWinning = true;
		}
		else  if (Tmode == 1) {
			// ������ʱ
			auto endTime_Re = chrono::system_clock::now();
			countdownData.used = endTime_Re - countdownData.startTime;
		}
		cout << "WOW~~ isWin!" << endl;
		showWin();
		cout << "showWin finished" << endl;


		lastFunctionTime = currentTime;
	}

	
	//��� X ��������ƶ�
	else if (GetAsyncKeyState('X') & 0x8000) {
		Debuff_jojo();
		lastFunctionTime = currentTime;
	}
	// ��� V �������˲���
	else if (GetAsyncKeyState('V') & 0x8000) {
		if (history.size() >= 3) {
			Buff_jojo(3);
		}
		else {
			killerQueen_de.play();
		}
		lastFunctionTime = currentTime;
	}
	//���C��:չʾͼƬ
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
			// �ָ���һ��״̬
			board = history.back();
			history.pop_back();
			moves--;

			// ���¿հ׿�λ��
			for (int i = 0; i < SSIZE; i++) {
				for (int j = 0; j < SSIZE; j++) {
					if (board[i][j] == 0) {
						emptyRow = i;
						emptyCol = j;
					}
				}
			}
		}

		//ˢ��

		drawGame();
		FlushBatchDraw();
	}
}



void initAnimations() {
	// ��ʼ��debuff����
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

	// ���ض�����Դ
	if (!DebuffAnimation.loadFrames()) {
		MessageBox(GetHWnd(), _T("Debuff������Դ����ʧ��"), _T("����"), MB_OK);
	}
	if (!display.loadFrames()) {
		MessageBox(GetHWnd(), _T("Debuff������Դ����ʧ��"), _T("����"), MB_OK);
	}
}

void Level_TE::Shuffle(int times) {
	srand((unsigned)time(NULL));
	int preDir[1][2];//��ǰ�ƶ��ķ�����
	int last = -1;//��ʼû����ǰ�ƶ�����
	// Ԥ�������п��ܵĺϷ��ƶ���
	for (int i = 0; i < times; i++) {
		// ��ȡ��ǰ�հ׿���Χ�ɽ�����λ��
		int neighbors[4][2] = {
			{emptyRow - 1, emptyCol}, // ��
			{emptyRow + 1, emptyCol}, // ��
			{emptyRow, emptyCol - 1}, // ��
			{emptyRow, emptyCol + 1}  // ��
		};

		// �ռ����кϷ����ھ�λ��
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
		// ���ѡ��һ���ھӽ��н���
		if (count > 0) {
			int idx = rand() % count;
			int targetRow = validNeighbors[idx][0];
			int targetCol = validNeighbors[idx][1];
			preDir[0][0] = emptyRow;
			preDir[0][1] = emptyCol;

			// ���浱ǰ״̬����ʷ��¼
			history.push_back(board);
			// �������50����ʷ��¼
			if (history.size() > 50) history.erase(history.begin());

			// ִ�н�����ģ���ƶ���
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
		getwidth() / 2 - 200,  // ��ʼX��ˮƽ���У�
		getheight() / 2 - 150, // ��ʼY����ֱ���У�
		getwidth() / 2 - 200,  // ����X��ˮƽ���У�
		getheight() / 2 - 150, // ����Y����ֱ���У�
		[](float progress, int& x, int& y, int sx, int sy, int ex, int ey) {
			PathFunctions::heartbeat(progress, x, y, ex, ey);
		},
		SRCCOPY
	);*/

	// ������Ļ����λ��
	int centerX = getwidth() / 2 - 200;  // �������400
	int centerY = getheight() / 2 - 150; // �����߶�300

	// ������ʼλ��Ϊ��Ļ���Ͻ��⣬����λ��Ϊ��Ļ����
	DebuffAnimation.play(
		getwidth(),          // ��ʼX����Ļ�Ҳ���
		-300,                // ��ʼY����Ļ�Ϸ���
		centerX,             // ����X��ˮƽ����
		centerY,             // ����Y����ֱ����
		PathFunctions::linear, // ʹ�������ƶ�·��
		SRCCOPY
	);

	Shuffle(3);
}

void Level_TE::initBoard() {
	Level::initBoard();
	// ��������͸����Ϊ255����ȫ��͸����
	for (int i = 0; i < SSIZE; i++) {
		for (int j = 0; j < SSIZE; j++) {
			alphaBoard[i][j] = 255;
		}
	}
}

void Level_TE::moveTile(int row, int col) {
	if (canMove(row, col)) {
		// ���浱ǰ״̬����ʷ��¼
		history.push_back(board);
		if (history.size() > 50) history.erase(history.begin());

		// ��ȡ���ƶ������ֵ
		int movedValue = board[row][col];

		// ����÷������ȷλ��
		int correctRow = (movedValue - 1) / SSIZE;
		int correctCol = (movedValue - 1) % SSIZE;

		// ����ƶ�ǰ�Ƿ�����ȷλ��
		bool wasInCorrectPosition = (row == correctRow && col == correctCol);

		// ���ٱ��ƶ����͸���ȣ������Ƿ�����ȷλ�ã�
		if (alphaBoard[row][col] > minAlpha) {
			alphaBoard[row][col] = minAlpha > (alphaBoard[row][col] - alphaDecrement) ? minAlpha : (alphaBoard[row][col] - alphaDecrement);
		}

		// ִ���ƶ�
		board[emptyRow][emptyCol] = movedValue;
		board[row][col] = 0;

		// �ƶ������Ƿ�����ȷλ��
		bool nowInCorrectPosition = (emptyRow == correctRow && emptyCol == correctCol);

		// ����͸�������� - ͸����Ӧ���淽���ƶ�
		BYTE movedAlpha = alphaBoard[row][col];
		alphaBoard[emptyRow][emptyCol] = nowInCorrectPosition ? 255 : movedAlpha;

		// ��������뿪��ȷλ�ã�������ԭʼλ�õ�͸����
		if (wasInCorrectPosition && !nowInCorrectPosition) {
			alphaBoard[row][col] = 255; // ����ԭʼλ�õ�͸����
		}

		// ���¿հ�λ��
		emptyRow = row;
		emptyCol = col;
		moves++;
	}
}

void Level_TE::drawGame() {
	setbkcolor(RGB(240, 240, 240));
	cleardevice();

	// ���Ʊ�����ƶ����������ֲ��䣩
	settextcolor(BLACK);
	settextstyle(30, 0, _T("����"));
	outtextxy(SSIZE * 50, 10, _T("���ֻ��ݵ�"));

	TCHAR movesText[50];
	_stprintf_s(movesText, _T("�ƶ�����: %d"), moves);
	settextstyle(20, 0, _T("����"));
	outtextxy(SSIZE * 50 + 50, 50, movesText);

	//���Ƽ�ʱʱ��
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
		_stprintf_s(timeText, _T("ʱ��: %02d:%02d"), minutes, seconds);
		outtextxy(SSIZE * 50 + 50, 80, timeText);  // ����λ�ñ����ص�
	}
	else if (Tmode == 1) {
		int minutes = countdownData.remaining / 60;
		int seconds = countdownData.remaining % 60;

		TCHAR timeText[50];
		_stprintf_s(timeText, _T("ʣ��ʱ��: %02d:%02d"), minutes, seconds);
		settextcolor(RED);  // �ú�ɫǿ��ʱ��
		outtextxy(SSIZE * 50 + 50, 80, timeText);
		settextcolor(BLACK); // �ָ�Ĭ����ɫ
	}

	// ������Ϸ���飨��͸���ȣ�
	for (int i = 0; i < SSIZE; i++) {
		for (int j = 0; j < SSIZE; j++) {
			int value = board[i][j];
			if (value == 0) continue; // �հ׿鲻����

			int x = MARGIN + j * BLOCK_SIZE;
			int y = MARGIN + i * BLOCK_SIZE + 80;
			BYTE alpha = alphaBoard[i][j];

			// ʹ��GDI+���ƴ�͸���ȵ�ͼ��
			HDC hdc = GetImageHDC();
			Graphics graphics(hdc);

			// ������ʱλͼ
			Bitmap bmp(BLOCK_SIZE, BLOCK_SIZE, PixelFormat32bppARGB);
			BitmapData bmpData;
			Gdiplus::Rect rect(0, 0, BLOCK_SIZE, BLOCK_SIZE);
			bmp.LockBits(&rect, ImageLockModeWrite, PixelFormat32bppARGB, &bmpData);

			// ��ȡԭʼͼƬ����
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

					// Ӧ�õ�ǰ͸����
					BYTE finalAlpha = (originalAlpha * alpha) / 255;

					// ���õ�Ŀ��λͼ
					pDst[0] = b;
					pDst[1] = g;
					pDst[2] = r;
					pDst[3] = finalAlpha;
					pDst += 4;
				}
			}
			bmp.UnlockBits(&bmpData);

			// ���Ƶ���Ļ
			graphics.DrawImage(&bmp, x, y);

			// ���Ʊ߿�
			setlinecolor(BLACK);
			rectangle(x, y, x + BLOCK_SIZE, y + BLOCK_SIZE);
		}
	}

	// ���Ʋ���˵�������ֲ��䣩
	settextstyle(16, 0, _T("����"));
	outtextxy(50, SSIZE * 100 + 150, _T("����˵��:"));
	outtextxy(50, SSIZE * 100 + 170, _T("����� - �ƶ�����"));
	outtextxy(50, SSIZE * 100 + 190, _T("R - ���¿�ʼ"));
	outtextxy(50, SSIZE * 100 + 210, _T("ESC - �˳���Ϸ"));
}