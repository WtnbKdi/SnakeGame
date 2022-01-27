#include <iostream>
#include <vector>
#include <windows.h>
#include <time.h>
#include <chrono>

using namespace std;

// 蛇の進行方向を表す
enum class SnakeVector
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
};

// 座標
struct Pos
{
	int X, Y;
};

// ゲーム情報
struct GameInfo 
{
	int	   Score = 0, 
		   SnakeLength = 1;
};

using v1c = vector<char>;
using v2c = vector<vector<char>>;
using vs = vector<Pos>;
using timep = chrono::system_clock::time_point;

// フィールドの上下の長さとその中心
const int FldWidth = 50, 
		  FldHeight = 20,
		  FldCenterWidth = FldWidth / 2,
		  FldCenterHeight = FldHeight / 2;

v2c Fld(FldHeight, v1c(FldWidth)); // フィールド

vs Snake;                     // 蛇本体
SnakeVector CurSnakeVec;      // 蛇の進行方向
Pos ItemPos;                  // 餌の座標
GameInfo gameInfo;            // ゲーム情報
bool GetItemFlg = false;      // 蛇が餌を獲得したか
const unsigned SnakeHead = 0; // 蛇の頭

COORD Crd{ 0, 0 };
CONSOLE_CURSOR_INFO Ini;
CONSOLE_CURSOR_INFO Cci{ 1, FALSE };

// フィールドの描画
void PrintField() 
{
	for (const auto& y : Fld) {
		for (const auto& x : y) {
			cout << x;
		}
		cout << "\n";
	}
}

// ゲーム情報更新
void PrintGameInfo() 
{
	cout << endl << "スコア: " << gameInfo.Score 
		 << " 蛇の長さ: " << gameInfo.SnakeLength;
}

// 餌を獲得したか
bool CheckGetItem() 
{
	return Snake[SnakeHead].X == ItemPos.X 
		&& Snake[SnakeHead].Y == ItemPos.Y;
}

// 餌を生成
void GenItem()
{
	ItemPos.X = (rand() % (FldWidth - 2)) + 1;
	ItemPos.Y = (rand() % (FldHeight - 2)) + 1;
	Fld[ItemPos.Y][ItemPos.X] = '@'; // フィールドに餌を設置
}

// フィールドを生成
void GenField() 
{
	for (int y = 0; y < FldHeight; y++) {
		for (int x = 0; x < FldWidth; x++) {
			if (y == 0 || y == FldHeight - 1) Fld[y][x] = '#'; // フィールドに上下の壁を設置
			else if (x == 0 || x == FldWidth - 1) Fld[y][x] = '#'; // フィールドに左右の壁を設置
			else Fld[y][x] = ' '; // 蛇が移動可能な場所
		}
	}
}

// キー入力取得
void SetSnakeVector()
{
	if (GetAsyncKeyState(VK_UP) & 1) CurSnakeVec = SnakeVector::UP;
	else if(GetAsyncKeyState(VK_DOWN) & 1) CurSnakeVec = SnakeVector::DOWN;
	else if (GetAsyncKeyState(VK_LEFT) & 1) CurSnakeVec = SnakeVector::LEFT;
	else if (GetAsyncKeyState(VK_RIGHT) & 1) CurSnakeVec = SnakeVector::RIGHT;
}

// 蛇をフィールドから削除
void SnakeClear() 
{
	for (const auto& sn : Snake) 
		Fld[sn.Y][sn.X] = ' ';
}

// 蛇のパーツが衝突したか
bool CheckHitSnakeParts()
{
	for (int j = 1; j < Snake.size(); j++) {
		if (Snake[SnakeHead].X == Snake[j].X && Snake[SnakeHead].Y == Snake[j].Y) return true;
	}
	return false;
}

// ゲームオーバー
void GameOver()
{
	string hoge;
	string gOverMsg = "<<<< Game Over >>>>";
	string tipMsg = "適当なキーを入力後Enterキーで終了します。";
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

// 蛇がフィールドアウトしたか
void FiledOutCheck()
{
	if (Snake[SnakeHead].X <= 0
		|| Snake[SnakeHead].X >= FldWidth - 1
		|| Snake[SnakeHead].Y <= 0
		|| Snake[SnakeHead].Y >= FldHeight - 1) {
		GameOver();
	}
}

// 蛇を伸ばす
void AddSnakeParts() 
{
	switch (CurSnakeVec) {
		case SnakeVector::UP: Snake.push_back({ Snake[Snake.size() - 1].X, Snake[Snake.size() - 1].Y - 1 }); break;
		case SnakeVector::DOWN: Snake.push_back({ Snake[Snake.size() - 1].X, Snake[Snake.size() - 1].Y + 1 }); break;
		case SnakeVector::LEFT: Snake.push_back({ Snake[Snake.size() - 1].X + 1, Snake[Snake.size() - 1].Y }); break;
		case SnakeVector::RIGHT: Snake.push_back({ Snake[Snake.size() - 1].X - 1, Snake[Snake.size() - 1].Y }); break;
	}
}

// 蛇の移動方向を変化させる
void SnakeHeadPosSet()
{
	switch (CurSnakeVec) {
		case SnakeVector::UP: Snake[SnakeHead].Y--; break;
		case SnakeVector::DOWN: Snake[SnakeHead].Y++; break;
		case SnakeVector::LEFT: Snake[SnakeHead].X--; break;
		case SnakeVector::RIGHT: Snake[SnakeHead].X++; break;
	}
}

// 蛇の後続を更新
void SnakeUpdate(vs snakeTmp)
{
	for (int i = 0; i < Snake.size() - 1; i++) 
		Snake[i + 1] = snakeTmp[i];
}

// 蛇を描画
void PrintSnake()
{
	for (auto sn : Snake) 
		Fld[sn.Y][sn.X] = 'O';
}

// 更新
void Update()
{
	vs snakeTmp = Snake; // 蛇の座標をバックアップ
	// ちらつき軽減
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Crd);
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &Ini);
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &Cci);
	
	FiledOutCheck(); // 蛇フィールドアウトのチェック
	SetSnakeVector(); // 蛇の進行方向のセット
	SnakeClear(); // 蛇をフィールドから消去
	SnakeHeadPosSet(); // 次の蛇の頭をセット
	if (CheckHitSnakeParts()) GameOver(); // 蛇の頭が胴体にぶつかっていないか
	if (CheckGetItem()) { // アイテム取得時の処理
		gameInfo.Score += 10;
		gameInfo.SnakeLength += 1;
		AddSnakeParts();
		GenItem();
	}
	SnakeUpdate(snakeTmp); // 蛇の後続を更新
	PrintSnake(); // 蛇を描画
	PrintField(); // フィールドを描画
	PrintGameInfo();
	//system("cls"); // 画面クリア

	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &Cci);
}

// 初期化
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

// 開始と現在時間の差を計算
long long diffTimeMilSec(const timep& startTime, const timep& nowTime)
{
	auto diff = nowTime - startTime;
	return chrono::duration_cast<chrono::milliseconds>(diff).count();
}

int main(void) 
{
	const float fps = 100; // ゲーム更新速度 ms
	auto startTime = chrono::system_clock::now(); // 開始時間
	Init(); // 初期化
	while (1) {
		auto nowTime = chrono::system_clock::now(); // 現在時間
		if (int diff = diffTimeMilSec(startTime, nowTime) >= fps) { // 開始現在時間の差がfps秒以上であれば更新
			Update();
			startTime = nowTime;
		}
	}
}