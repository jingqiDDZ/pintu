#include "level.h"

static int value_6 = Level_6::Prob;

void Level_6::Buff_jojo(int n){
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

void Level_6::Debuff_jojo(){
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
	debuffAnimation.play(
		getwidth(),          // ��ʼX����Ļ�Ҳ���
		-300,                // ��ʼY����Ļ�Ϸ���
		centerX,             // ����X��ˮƽ����
		centerY,             // ����Y����ֱ����
		PathFunctions::linear, // ʹ�������ƶ�·��
		SRCCOPY
	);

	Shuffle(3);
}

void Level_6::Shuffle(int times){
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

int Level_6::handleFunctionKeys() {
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
	//���˲���
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

LevelResult Level_6::play(){
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
		if (display.isPlaying() && display.getType() == Animation::NON_BLOCKING) {
			display.updateNonBlocking();
		}

		/*// ����Ƿ������������ڲ��ţ���debuffAnimation��
		bool isBlockingAnimation = debuffAnimation.isPlaying() &&
			(debuffAnimation.getType() == Animation::BLOCKING);
		*/
		// �Ƕ����ڼ䴦������
		//if (!isBlockingAnimation) {
		if (handleMouse() || handleKeyboard()) {
			sound_click.play();
		}

		//�����ܼ�
		int funcReturn = handleFunctionKeys();
		if (funcReturn == 1) {
			return LevelResult::Exit;
		}
		//}

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

void Level_6::moveTile(int row, int col) {
	if (canMove(row, col)) {

		// ���浱ǰ״̬����ʷ��¼
		history.push_back(board);
		// �������50����ʷ��¼
		if (history.size() > 50) history.erase(history.begin());

		board[emptyRow][emptyCol] = board[row][col];
		board[row][col] = 0;
		emptyRow = row;
		emptyCol = col;
		moves++;
		value_6 = (value_6 - stepsize) < Minvalue_Prob ? Minvalue_Prob : (value_6 - stepsize);
		int r = rand() % value_6;
		if (r == 1) {
			Debuff_jojo();
		}
		if (value_6 == Minvalue_Prob) value_6 = Prob;
	}
}