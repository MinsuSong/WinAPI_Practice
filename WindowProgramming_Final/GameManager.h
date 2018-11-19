#pragma once
#include <windows.h>
#include <atlimage.h>
#include <math.h>
#include <chrono>
#include "MyVector.h"

/*
	������ ��ǥ���� ������ �»���̱� ������
	����ǥ�� �������� ȸ������ �ָ� �ð�������� 3�ÿ��� 6�� - 9�� - 12�� �������� ���� �����Ѵ�.
*/

//	Window Size
#define WindowWidth			1000
#define WindowHeight		800

//	Child Window Size
#define ChildWindowSize		752		/* gm.board�� 750�� ���簢������ ������ֱ� ���ؼ� �ϵ��ڵ�����! */

//	CannonSize
#define CannonImgSize		60		//	square

//	RectSize	/* ĳ���Ͷ� ����ü ������ �Ȱ��� */
#define CharSize			8

//	Background Rect Size
#define BGSize				80

/* Logo �̹��� ��¿� DEFINES */
//	Logo Image Size (750 * 3) x 750
#define LgSize				750
//	Keyboard Image Size (132 * 3) x 88, Keyboard img location == LT(64, 604)
#define KbWidth				132
#define KbHeight			88

//	Amount of Enemy

//	MATH DEFINES of mine! 
/* ����� ���� ��ġ�� */
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

	void countframe(void)	//	�̿ϼ�
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
	/* ��� �簢���� */
	RECT r[8];
	POINT p[8];
};

struct Enemy {
	POINT loc{ 0 };
	POINT rect[4]{ 0 };
	VECTOR v{ 0 };				//	���⺤��	
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
		/* �ǹ� 1 . �̰� ���� ���⺤�͸� ����°���? �ƴϸ� ����ȭ�� �ȵȰǰ�, ����ǥ�� �������� r==1�̰� �����޾����ϱ� ����ȭ��
		���⺤���� ������ ������. */
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
	POINT rect[4]{ 0 }; /* 0������ LT, RT, RB, LB �ð���� */
	VECTOR v{ 0, -1 }; /* x �࿡ ������ ���� ���� �ַ��� �ǵ����� */
	int degree{ 270 }; /* ���� ������ ����, �����찡 LT�� �����̴ϱ� */
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
		setVector(v, degree); /* ȸ���� ���� ���� ���͸� �������ش�. */
	}
};

bool inRect(const POINT& p, const RECT& r);
bool inRect(const POINT& p, const RECT& r, const RECT& r2);
void getCannonPointRotated(POINT* p, int degree, const RECT& board, int direction);
void getPointRotate(POINT* rect, int degree, const POINT& loc);
COLORREF& makeReverseRGB(COLORREF& rgb);
