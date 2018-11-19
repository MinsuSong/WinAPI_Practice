#pragma once
#include <windows.h>
#include <atlimage.h>
#include <math.h>
#include <chrono>
#include "MyVector.h"

/*
	윈도우 좌표계의 원점은 좌상단이기 때문에
	극좌표계 기준으로 회전각을 주면 시계방향으로 3시에서 6시 - 9시 - 12시 방향으로 돌기 시작한다.
*/

//	Window Size
#define WindowWidth			1000
#define WindowHeight		800

//	Child Window Size
#define ChildWindowSize		752		/* gm.board를 750의 정사각형으로 만들어주기 위해서 하드코딩했음! */

//	CannonSize
#define CannonImgSize		60		//	square

//	RectSize	/* 캐릭터랑 적개체 사이즈 똑같게 */
#define CharSize			8

//	Background Rect Size
#define BGSize				80

/* Logo 이미지 출력용 DEFINES */
//	Logo Image Size (750 * 3) x 750
#define LgSize				750
//	Keyboard Image Size (132 * 3) x 88, Keyboard img location == LT(64, 604)
#define KbWidth				132
#define KbHeight			88

//	Amount of Enemy

//	MATH DEFINES of mine! 
/* 사용자 정의 수치값 */
#define CLOCKWISE			1
#define ANTI_CLOCKWISE		-1

struct GameManager {
	HDC hdc, hmemdc;
	HBITMAP hmembit, oldmembit;
	HBRUSH hbrush, oldbrush;
	HPEN hpen, oldpen;
	RECT board{ 0 };
	bool isStarted{ false };
	int frame = 0;
	std::chrono::duration<double> frameTime;
	std::chrono::system_clock::time_point startTime;
	std::chrono::duration<double> elapsedTime;

	void countframe(void)	//	미완성
	{
		if (frame == 3) {
			frame = 0;
		}
	}
};

struct ButtonManager {
	bool isLeft{ false };
	bool isRight{ false };
	bool isUp{ false };
	bool isDown{ false };
};

struct LogoImage {
	CImage logo;
	CImage keyboard;
	int frame{ 0 };
};

struct BackGround {
	/* 배경 사각형들 */
	RECT r[8];
	POINT p[8];
};

struct Enemy {
	POINT loc{ 0 };
	POINT rect[4]{ 0 };
	VECTOR v{ 0 };				//	방향벡터	
	double speed{ 0 };
	double accelation{ 0 };
	void draw(GameManager& gm) {
		gm.hbrush = CreateSolidBrush(RGB(115, 213, 209));
		gm.oldbrush = (HBRUSH)SelectObject(gm.hmemdc, gm.hbrush);
		
		SelectObject(gm.hmemdc, gm.oldbrush); DeleteObject(gm.hbrush);
	}
	void setVector(VECTOR& v, const GameManager& gm, int degree) {
		double tmpRad = makeDegreeToRadian(degree);
		v.x = cos(tmpRad);
		v.y = sin(tmpRad);
	}
};

struct Cannon {
	CImage img;
	POINT loc[3]{ 0 };
	RECT range{ 0 };
	int currDegree{ 0 };
	VECTOR currV{ 0 };
	int targetDegree{ 0 };
	VECTOR targetV{ 0 };

	void setVector(VECTOR& v, const GameManager& gm, int degree) {
		double tmpRad = makeDegreeToRadian(degree);
		v.x = cos(tmpRad);
		v.y = sin(tmpRad);
		/* 의문 1 . 이게 지금 방향벡터를 내뱉는건지? 아니면 정규화가 안된건가, 극좌표계 기준으로 r==1이고 각도받았으니까 정규화된
		방향벡터일 것으로 생각됨. */
	}

	void locInitiate(const GameManager& gm) {
		loc[0].x = gm.board.right / 2 - CannonImgSize / 2;
		loc[0].y = gm.board.bottom / 2 - CannonImgSize / 2;
		loc[1].x = gm.board.right / 2 + CannonImgSize / 2;
		loc[1].y = gm.board.bottom / 2 - CannonImgSize / 2;
		loc[2].x = gm.board.right / 2 - CannonImgSize / 2;
		loc[2].y = gm.board.bottom / 2 + CannonImgSize / 2;
	}
	void InitiateCannon(const GameManager& gm) {
		locInitiate(gm);
		currDegree = 0;
		targetDegree = 0;
		range.left = loc[0].x;
		range.top = loc[0].y;
		range.right = loc[1].x;
		range.bottom = loc[2].y;
	}	
};

struct Character {
	POINT loc{ 0 };
	POINT rect[4]{ 0 }; /* 0번부터 LT, RT, RB, LB 시계방향 */
	VECTOR v{ 0, -1 }; /* x 축에 수직인 방향 벡터 주려고 의도했음 */
	int degree{ 270 }; /* 시작 방향의 각도, 윈도우가 LT이 원점이니까 */
	int speed{ 3 };
	ButtonManager bm;
	void Initiate(const GameManager& gm) {
		loc.x = gm.board.right / 2;
		loc.y = gm.board.bottom * 3 / 4;
	}
	void draw(GameManager& gm) {
		gm.hbrush = CreateSolidBrush(RGB(255, 36, 216));
		gm.oldbrush = (HBRUSH)SelectObject(gm.hmemdc, gm.hbrush);
		Polygon(gm.hmemdc, rect, 4);
		SelectObject(gm.hmemdc, gm.oldbrush); DeleteObject(gm.hbrush);
	}
	void setVector(VECTOR& v, int degree) {
		double tmpRad = makeDegreeToRadian(degree);
		v.x = cos(tmpRad);
		v.y = sin(tmpRad);
	}
	void doMove(const GameManager& gm) {
		if (bm.isUp) {
			{
				if (gm.board.left <= loc.x - CharSize && loc.x + CharSize <= gm.board.right) {
					loc.x += v.x * speed;
				}
				else if (gm.board.left >= loc.x - CharSize) {
					loc.x = gm.board.left + CharSize;
				}
				else if (gm.board.right <= loc.x + CharSize) {
					loc.x = gm.board.right - CharSize;
				}
			}
			{
				if (gm.board.top <= loc.y - CharSize && loc.y + CharSize <= gm.board.bottom) {
					loc.y += v.y * speed;
				}
				else if (gm.board.top >= loc.y - CharSize) {
					loc.y = gm.board.top + CharSize;
				}
				else if (gm.board.bottom <= loc.y + CharSize) {
					loc.y = gm.board.bottom - CharSize;
				}
			}
		}
		else if (bm.isDown) {
			{
				if (gm.board.left <= loc.x - CharSize && loc.x + CharSize <= gm.board.right) {
					loc.x -= v.x * speed;
				}
				else if (gm.board.left >= loc.x - CharSize) {
					loc.x = gm.board.left + CharSize;
				}
				else if (gm.board.right <= loc.x + CharSize) {
					loc.x = gm.board.right - CharSize;
				}
			}
			{
				if (gm.board.top <= loc.y - CharSize && loc.y + CharSize <= gm.board.bottom) {
					loc.y -= v.y * speed;
				}
				else if (gm.board.top >= loc.y - CharSize) {
					loc.y = gm.board.top + CharSize;
				}
				else if (gm.board.bottom <= loc.y + CharSize) {
					loc.y = gm.board.bottom - CharSize;
				}
			}
		}
	}


	void doRotate(const GameManager& gm) {
		if (0 <= degree && degree <= 360) {
			if (bm.isLeft) {
				degree -= 5;
			}
			else if (bm.isRight) {
				degree += 5;
			}
		}
		else if (degree < 0) {
			degree = 360;
		}
		else if (degree > 360) {
			degree = 0;
		}
		setVector(v, degree); /* 회전된 각에 의한 벡터를 수정해준다. */
	}
};

bool inRect(const POINT& p, const RECT& r);
bool inRect(const POINT& p, const RECT& r, const RECT& r2);
void getCannonPointRotated(POINT* p, int degree, const RECT& board, int direction);
void getPointRotate(POINT* rect, int degree, const POINT& loc);
COLORREF& makeReverseRGB(COLORREF& rgb);
