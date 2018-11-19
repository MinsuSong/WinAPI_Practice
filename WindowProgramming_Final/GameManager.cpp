#include "GameManager.h"

bool inRect(const POINT& p, const RECT& r)
{
	/* ���� �浹 ������ true�� ��ȯ�մϴ�. */
	if ((r.left <= p.x&&p.x <= r.right) && (r.top <= p.y&&p.y <= r.bottom)) {
		return true;
	}
	return false;
}

bool inRect(const POINT& p, const RECT& r, const RECT& r2)
{
	/* ���� �浹 ������ true�� ��ȯ�մϴ�. */
	if ((r.left <= p.x&&p.x <= r.right) && (r2.top <= p.y&&p.y <= r2.bottom)) {
		return true;
	}
	return false;
}

void getCannonPointRotated(POINT* p, int degree, const RECT& board, int direction)
{
	p[0].x = board.right / 2 - 30;
	p[0].y = board.bottom / 2 - 30;
	p[1].x = board.right / 2 + 30;
	p[1].y = board.bottom / 2 - 30;
	p[2].x = board.right / 2 - 30;
	p[2].y = board.bottom / 2 + 30;
	/* p �� ��� �ʱ�ȭ ������� �簢�� ���¸� ������ �����ָ� ���� ���� �ҽǵǴ� ���� �߰� �Ƹ� �ε��Ҽ���� ������ */
	long oldx{ 0 };
	long oldy{ 0 };
	/* �ﰢ�Լ��� �̿��� ���� ��, ó���� p[i] x,y ���� ������ִ� old ���� */
	double radian = direction * degree * PI / 180;
	for (int i = 0; i < 3; ++i) {
		oldx = p[i].x, oldy = p[i].y;
		p[i].x = (oldx - board.right / 2) * cos(radian) - (oldy - board.bottom / 2) * sin(radian) + board.right / 2;
		p[i].y = (oldx - board.right / 2) * sin(radian) + (oldy - board.bottom / 2) * cos(radian) + board.bottom / 2;
	}
}


void getPointRotate(POINT* rect, int degree, const POINT& loc)
{
	rect[0].x = loc.x - CharSize;
	rect[0].y = loc.y - CharSize;
	rect[1].x = loc.x + CharSize;
	rect[1].y = loc.y - CharSize;
	rect[2].x = loc.x + CharSize;
	rect[2].y = loc.y + CharSize;
	rect[3].x = loc.x - CharSize;
	rect[3].y = loc.y + CharSize;
	long oldx{ 0 }, oldy{ 0 };
	double rad = makeDegreeToRadian(degree);
	for (int i = 0; i < 4; ++i) {
		oldx = rect[i].x, oldy = rect[i].y;
		rect[i].x = (oldx - loc.x)* cos(rad) - (oldy - loc.y)* sin(rad) + loc.x;
		rect[i].y = (oldx - loc.x)*sin(rad) + (oldy - loc.y)* cos(rad) + loc.y;
	}
}

COLORREF& makeReverseRGB(COLORREF& rgb)
{
	rgb = RGB(255 - GetRValue(rgb), 255 - GetGValue(rgb), 255 - GetBValue(rgb));
	return rgb;
}
