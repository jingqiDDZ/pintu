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



	// ���ض�����Դ
	if (!display1.loadFrames()) {
		MessageBox(GetHWnd(), _T("ͼƬ������Դ����ʧ��"), _T("����"), MB_OK);
	}
	if (!display2.loadFrames()) {
		MessageBox(GetHWnd(), _T("ͼƬ������Դ����ʧ��"), _T("����"), MB_OK);
	}
	if (!debuffAnimation.loadFrames()) {
		MessageBox(GetHWnd(), _T("Debuff������Դ����ʧ��"), _T("����"), MB_OK);
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

void Level_7::Debuff_jojo() {
	kingCrimson.play();

	debuffAnimation.setAlpha(250);
	debuffAnimation.setStayDuration(1000);

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
	/*debuffAnimation.play(
		getwidth(),          // startX
		-300,                // startY
		centerX,             // endX
		centerY,             // endY
		[](float progress, int& x, int& y, int sx, int sy, int ex, int ey) {
			// ʹ��lambda��ȷת������
			PathFunctions::linear(progress, x, y, sx, sy, ex, ey);
		},  // ��ȷ����6������
		SRCCOPY
	);*/
	debuffAnimation.startNonBlocking(centerX, centerY, centerX, centerY);

	Shuffle(3);
}

void Level_7::Shuffle(int times) {
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

int Level_7::handleFunctionKeys() {
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
		//showWin();

		// ��һ�׶�ʤ�������ڶ��׶�
		if (stage == 1) {
			stage = 2;
			win_1 = true;
			trans.play();
			// ������������
			//debuffAnimation.play(...); // ������Զ��嶯������
			// �л���Դ
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
	//���˲���
	/*else if (GetAsyncKeyState('Q') & 0x8000) {
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

			// ������ʷ״̬
			history.push_back(board);

			// ����͸���� (���ƶ��ķ���)
			if (alphaBoard[row][col] > minAlpha) {
				alphaBoard[row][col] =
					(alphaBoard[row][col] - alphaDecrement < minAlpha) ?
					minAlpha : (alphaBoard[row][col] - alphaDecrement);
			}

			// �����ƶ�ǰ�Ŀհ�λ��
			int oldEmptyRow = emptyRow;
			int oldEmptyCol = emptyCol;

			// �ƶ�����
			swap(board[emptyRow][emptyCol], board[row][col]);
			swap(alphaBoard[emptyRow][emptyCol], alphaBoard[row][col]);

			// ���¿հ�λ��
			emptyRow = row;
			emptyCol = col;
			moves++;

			// +++ ����������ƶ���ķ����Ƿ�����ȷλ�� +++
			int movedValue = board[oldEmptyRow][oldEmptyCol];  // ���ƶ��ķ���ֵ
			if (movedValue != 0) {
				// ������ȷλ�ã��к��У�
				int correctRow = (movedValue - 1) / SSIZE;
				int correctCol = (movedValue - 1) % SSIZE;

				// �����ǰ����ȷλ�ã�����͸����
				if (oldEmptyRow == correctRow && oldEmptyCol == correctCol) {
					alphaBoard[oldEmptyRow][oldEmptyCol] = 255;  // ��ȫ͸��
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
		if (display1.isPlaying() && display1.getType() == Animation::NON_BLOCKING && stage==1) {
			display1.updateNonBlocking();
		}
		if (display2.isPlaying() && display2.getType() == Animation::NON_BLOCKING && stage==2) {
			display2.updateNonBlocking();
		}
		if (debuffAnimation.isPlaying() && debuffAnimation.getType() == Animation::NON_BLOCKING) {
			debuffAnimation.updateNonBlocking();
		}

		// ����Ƿ������������ڲ��ţ���debuffAnimation��
		/*bool isBlockingAnimation = debuffAnimation.isPlaying() &&
			(debuffAnimation.getType() == Animation::BLOCKING);*/

		// �Ƕ����ڼ䴦������
		//if (!isBlockingAnimation) {
			if (handleMouse() || handleKeyboard()) {
				sound_click.play();
			}

			//�����ܼ�
			int funcReturn = handleFunctionKeys();
			if (funcReturn == 1) {
				return LevelResult::Exit;
		//	}
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

			// ��һ�׶�ʤ�������ڶ��׶�
			if (stage == 1) {
				stage = 2;
				win_1 = true;
				trans.play();
				// ������������
				//debuffAnimation.play(...); // ������Զ��嶯������
				// �л���Դ
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
			// �����������Լ��Ļ����߼��������������
			continue;
		}*/

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
				else if (GetAsyncKeyState(VK_ESCAPE) & 0x8008) {
					return LevelResult::Exit;
				}

				// ����CPUռ�ù���
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
		// ��ʼ��͸����
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

	// ���Ʊ�����ƶ����������ֲ��䣩
	settextcolor(BLACK);
	settextstyle(30, 0, _T("����"));
	outtextxy(SSIZE * 50, 10, _T("���ֻ��ݵ�"));

	TCHAR movesText[50];
	_stprintf_s(movesText, _T("�ƶ�����: %d"), moves);
	settextstyle(20, 0, _T("����"));
	outtextxy(SSIZE * 50 + 50, 50, movesText);

	// ���Ƽ�ʱʱ��
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
			if (stage == 2) {
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
			}
			else {
				putimage(x, y, &blockImgs[board[i][j]]);//������ͼƬ���
			}
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

	// ���Ƶ�ǰ�׶���ʾ
	TCHAR stageText[50];
	_stprintf_s(stageText, _T("��ǰ�׶�: %d"), stage);
	outtextxy(SSIZE * 50 + 50, 120, stageText);
}