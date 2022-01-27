#include <iostream>
#include <vector>
#include <windows.h>
#include <time.h>
#include <chrono>

using namespace std;

// �ւ̐i�s������\��
enum class SnakeVector
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
};

// ���W
struct Pos
{
	int X, Y;
};

// �Q�[�����
struct GameInfo 
{
	int	   Score = 0, 
		   SnakeLength = 1;
};

using v1c = vector<char>;
using v2c = vector<vector<char>>;
using vs = vector<Pos>;
using timep = chrono::system_clock::time_point;

// �t�B�[���h�̏㉺�̒����Ƃ��̒��S
const int FldWidth = 50, 
		  FldHeight = 20,
		  FldCenterWidth = FldWidth / 2,
		  FldCenterHeight = FldHeight / 2;

v2c Fld(FldHeight, v1c(FldWidth)); // �t�B�[���h

vs Snake;                     // �֖{��
SnakeVector CurSnakeVec;      // �ւ̐i�s����
Pos ItemPos;                  // �a�̍��W
GameInfo gameInfo;            // �Q�[�����
bool GetItemFlg = false;      // �ւ��a���l��������
const unsigned SnakeHead = 0; // �ւ̓�

COORD Crd{ 0, 0 };
CONSOLE_CURSOR_INFO Ini;
CONSOLE_CURSOR_INFO Cci{ 1, FALSE };

// �t�B�[���h�̕`��
void PrintField() 
{
	for (const auto& y : Fld) {
		for (const auto& x : y) {
			cout << x;
		}
		cout << "\n";
	}
}

// �Q�[�����X�V
void PrintGameInfo() 
{
	cout << endl << "�X�R�A: " << gameInfo.Score 
		 << " �ւ̒���: " << gameInfo.SnakeLength;
}

// �a���l��������
bool CheckGetItem() 
{
	return Snake[SnakeHead].X == ItemPos.X 
		&& Snake[SnakeHead].Y == ItemPos.Y;
}

// �a�𐶐�
void GenItem()
{
	ItemPos.X = (rand() % (FldWidth - 2)) + 1;
	ItemPos.Y = (rand() % (FldHeight - 2)) + 1;
	Fld[ItemPos.Y][ItemPos.X] = '@'; // �t�B�[���h�ɉa��ݒu
}

// �t�B�[���h�𐶐�
void GenField() 
{
	for (int y = 0; y < FldHeight; y++) {
		for (int x = 0; x < FldWidth; x++) {
			if (y == 0 || y == FldHeight - 1) Fld[y][x] = '#'; // �t�B�[���h�ɏ㉺�̕ǂ�ݒu
			else if (x == 0 || x == FldWidth - 1) Fld[y][x] = '#'; // �t�B�[���h�ɍ��E�̕ǂ�ݒu
			else Fld[y][x] = ' '; // �ւ��ړ��\�ȏꏊ
		}
	}
}

// �L�[���͎擾
void SetSnakeVector()
{
	if (GetAsyncKeyState(VK_UP) & 1) CurSnakeVec = SnakeVector::UP;
	else if(GetAsyncKeyState(VK_DOWN) & 1) CurSnakeVec = SnakeVector::DOWN;
	else if (GetAsyncKeyState(VK_LEFT) & 1) CurSnakeVec = SnakeVector::LEFT;
	else if (GetAsyncKeyState(VK_RIGHT) & 1) CurSnakeVec = SnakeVector::RIGHT;
}

// �ւ��t�B�[���h����폜
void SnakeClear() 
{
	for (const auto& sn : Snake) 
		Fld[sn.Y][sn.X] = ' ';
}

// �ւ̃p�[�c���Փ˂�����
bool CheckHitSnakeParts()
{
	for (int j = 1; j < Snake.size(); j++) {
		if (Snake[SnakeHead].X == Snake[j].X && Snake[SnakeHead].Y == Snake[j].Y) return true;
	}
	return false;
}

// �Q�[���I�[�o�[
void GameOver()
{
	string hoge;
	string gOverMsg = "<<<< Game Over >>>>";
	string tipMsg = "�K���ȃL�[����͌�Enter�L�[�ŏI�����܂��B";
	const int gOverMsgPrintPoint = FldCenterWidth - gOverMsg.length() / 2;
	const int tipMsgPrintPoint = FldCenterWidth - tipMsg.length() / 2;
	for (int i = 0; i < gOverMsg.size(); i++)
		Fld[FldCenterHeight][gOverMsgPrintPoint + i] = gOverMsg[i];
	for (int i = 0; i < tipMsg.size(); i++)
		Fld[FldCenterHeight + 1][tipMsgPrintPoint + i] = tipMsg[i];
	PrintField();
	cin >> hoge;
	exit(0);
}

// �ւ��t�B�[���h�A�E�g������
void FiledOutCheck()
{
	if (Snake[SnakeHead].X <= 0
		|| Snake[SnakeHead].X >= FldWidth - 1
		|| Snake[SnakeHead].Y <= 0
		|| Snake[SnakeHead].Y >= FldHeight - 1) {
		GameOver();
	}
}

// �ւ�L�΂�
void AddSnakeParts() 
{
	switch (CurSnakeVec) {
		case SnakeVector::UP: Snake.push_back({ Snake[Snake.size() - 1].X, Snake[Snake.size() - 1].Y - 1 }); break;
		case SnakeVector::DOWN: Snake.push_back({ Snake[Snake.size() - 1].X, Snake[Snake.size() - 1].Y + 1 }); break;
		case SnakeVector::LEFT: Snake.push_back({ Snake[Snake.size() - 1].X + 1, Snake[Snake.size() - 1].Y }); break;
		case SnakeVector::RIGHT: Snake.push_back({ Snake[Snake.size() - 1].X - 1, Snake[Snake.size() - 1].Y }); break;
	}
}

// �ւ̈ړ�������ω�������
void SnakeHeadPosSet()
{
	switch (CurSnakeVec) {
		case SnakeVector::UP: Snake[SnakeHead].Y--; break;
		case SnakeVector::DOWN: Snake[SnakeHead].Y++; break;
		case SnakeVector::LEFT: Snake[SnakeHead].X--; break;
		case SnakeVector::RIGHT: Snake[SnakeHead].X++; break;
	}
}

// �ւ̌㑱���X�V
void SnakeUpdate(vs snakeTmp)
{
	for (int i = 0; i < Snake.size() - 1; i++) 
		Snake[i + 1] = snakeTmp[i];
}

// �ւ�`��
void PrintSnake()
{
	for (auto sn : Snake) 
		Fld[sn.Y][sn.X] = 'O';
}

// �X�V
void Update()
{
	vs snakeTmp = Snake; // �ւ̍��W���o�b�N�A�b�v
	// ������y��
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Crd);
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &Ini);
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &Cci);
	
	FiledOutCheck(); // �փt�B�[���h�A�E�g�̃`�F�b�N
	SetSnakeVector(); // �ւ̐i�s�����̃Z�b�g
	SnakeClear(); // �ւ��t�B�[���h�������
	SnakeHeadPosSet(); // ���̎ւ̓����Z�b�g
	if (CheckHitSnakeParts()) GameOver(); // �ւ̓������̂ɂԂ����Ă��Ȃ���
	if (CheckGetItem()) { // �A�C�e���擾���̏���
		gameInfo.Score += 10;
		gameInfo.SnakeLength += 1;
		AddSnakeParts();
		GenItem();
	}
	SnakeUpdate(snakeTmp); // �ւ̌㑱���X�V
	PrintSnake(); // �ւ�`��
	PrintField(); // �t�B�[���h��`��
	PrintGameInfo();
	//system("cls"); // ��ʃN���A

	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &Cci);
}

// ������
void Init() 
{
	srand((unsigned int)time(NULL));
	CurSnakeVec = SnakeVector::UP;
	Snake = { 
		{ FldCenterWidth,  FldCenterHeight } 
	};
	GenField();
	GenItem();
}

// �J�n�ƌ��ݎ��Ԃ̍����v�Z
long long diffTimeMilSec(const timep& startTime, const timep& nowTime)
{
	auto diff = nowTime - startTime;
	return chrono::duration_cast<chrono::milliseconds>(diff).count();
}

int main(void) 
{
	const float fps = 100; // �Q�[���X�V���x ms
	auto startTime = chrono::system_clock::now(); // �J�n����
	Init(); // ������
	while (1) {
		auto nowTime = chrono::system_clock::now(); // ���ݎ���
		if (int diff = diffTimeMilSec(startTime, nowTime) >= fps) { // �J�n���ݎ��Ԃ̍���fps�b�ȏ�ł���΍X�V
			Update();
			startTime = nowTime;
		}
	}
}