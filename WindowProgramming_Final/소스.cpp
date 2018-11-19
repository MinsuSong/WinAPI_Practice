#include <windows.h>
#include <tchar.h>
#include <random>
#include <iostream>
#include <math.h>
#include <fmod.h>


//   Window Size
#define WindowWidth         1000
#define WindowHeight      800

//   Child Window Size
#define ChildWindowSize      750

HWND g_hParent;
HWND g_hChild;      //   차일드 핸들 글로벌
HINSTANCE g_hInst;   //   인스턴스 핸들 글로벌
LPCTSTR lpszClass1 = "ParentClass";
LPCTSTR lpszClass2 = "ChildClass";
LPCTSTR lpszClass3 = "ChildClass2";


static int Char_PosX; //주인공 x y
static int Char_PosY;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ChildProc(HWND hCWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
bool inRect(const POINT& p, const RECT& r);
BOOL coll(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	FMOD_SYSTEM* soundSystem;
	FMOD_System_Create(&soundSystem);

	FMOD_System_Init(soundSystem, 10, FMOD_INIT_NORMAL, NULL);

	FMOD_SOUND *soundFile;
	FMOD_CHANNEL *channel;

	FMOD_System_CreateSound(soundSystem, "Sound.mp3", FMOD_LOOP_NORMAL, 0, &soundFile);

	FMOD_System_PlaySound(soundSystem, soundFile, NULL, 0, &channel);


	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;

	// 부모
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass1;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&WndClass);

	// 자식
	WndClass.hCursor = LoadCursor(NULL, IDC_CROSS);
	//   커서 없애줘야함.
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszClassName = lpszClass2;
	WndClass.lpfnWndProc = (WNDPROC)ChildProc;

	RegisterClassEx(&WndClass);

	g_hParent = CreateWindow(
		lpszClass1,
		"과제TEST",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,
		0, 0,
		WindowWidth, WindowHeight,
		NULL,
		(HMENU)NULL,
		hInstance,
		NULL
	);

	ShowWindow(g_hParent, nCmdShow);
	UpdateWindow(g_hParent);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{

	PAINTSTRUCT ps;
	HDC hdc, hmemdc;
	static HBITMAP hmembitmap, oldmembitmap;
	
	static RECT board;   //   실제 보드
	static POINT mouse;   //   마우스

	switch (iMessage)
	{
	case WM_CREATE:
		GetClientRect(hWnd, &board);
		g_hChild = CreateWindow("ChildClass", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
			board.right / 2 - ChildWindowSize / 2, board.bottom / 2 - ChildWindowSize / 2, ChildWindowSize, ChildWindowSize, hWnd, NULL, g_hInst, NULL);

		Char_PosX = 500;
		Char_PosY = 500;
		break;

	case WM_LBUTTONDOWN:
		mouse.x = LOWORD(lParam);
		mouse.y = HIWORD(lParam);
		break;

	case WM_KEYUP:
		break;

	case WM_KEYDOWN:
		switch (wParam) {
		case VK_LEFT:
			Char_PosX = Char_PosX - 10;
			break;
		case VK_RIGHT:
			Char_PosX = Char_PosX + 10;
			break;
		case VK_UP:
			Char_PosY = Char_PosY - 10;
			break;
		case VK_DOWN:
			Char_PosY = Char_PosY + 10;
			break;
		default:
			break;
		}
		InvalidateRect(hWnd, NULL, false);
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		if (hmembitmap == NULL) {
			hmembitmap = CreateCompatibleBitmap(hdc, board.right, board.bottom);
		}
		hmemdc = CreateCompatibleDC(hdc);      //   메인버퍼 호환되는 백버퍼
		oldmembitmap = (HBITMAP)SelectObject(hmemdc, (HBITMAP)hmembitmap);

		//   draw background
		Rectangle(hmemdc, board.right / 2 - 8, board.bottom / 2 - 8, board.right / 2 + 8, board.bottom / 2 + 8);

		//   draw on main dc
		BitBlt(hdc, board.left, board.top, board.right, board.bottom, hmemdc, 0, 0, SRCCOPY);

		SelectObject(hmemdc, oldmembitmap); DeleteDC(hmemdc);

		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, iMessage, wParam, lParam);
}

LRESULT CALLBACK ChildProc(HWND hCWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc, hmemdc;
	static HBITMAP hmembitmap, oldmembitmap;
	static HBRUSH hbrush, oldbrush;

	static RECT board;   //   실제 보드
	static RECT mouse{ 0 };   //   마우스



	static POINT p[8];
	static int size = 80;
	static int flag = 0;

	switch (iMessage)
	{
	case WM_CREATE:
		GetClientRect(hCWnd, &board);

		for (int i = 0; i < 8; i++)
		{
			p[i].x = board.right / 2 - (10 + 20 * 3 + 210) + size* i;
			p[i].y = board.bottom / 2 - (10 + 20 * 3 + 210) + size* i;
		}

		break;

	case WM_TIMER:
		switch (wParam)
		{
		case 1:

			break;
		default:
			break;
		}
		InvalidateRect(hCWnd, NULL, false);
		break;

	case WM_PAINT:
		hdc = BeginPaint(hCWnd, &ps);

		if (hmembitmap == NULL) {
			hmembitmap = CreateCompatibleBitmap(hdc, board.right, board.bottom);
		}

		hmemdc = CreateCompatibleDC(hdc);      //   메인버퍼 호환되는 백버퍼

		oldmembitmap = (HBITMAP)SelectObject(hmemdc, (HBITMAP)hmembitmap);

		//   배경색
		hbrush = (HBRUSH)CreateSolidBrush(RGB(220, 254, 255));
		oldbrush = (HBRUSH)SelectObject(hmemdc, hbrush);
		Rectangle(hmemdc, board.left, board.top, board.right, board.bottom);


		hbrush = (HBRUSH)CreateSolidBrush(RGB(142, 196, 220));
		oldbrush = (HBRUSH)SelectObject(hmemdc, hbrush);
		for (int j = 0; j < 8; j++)
		{
			for (int i = 0; i < 8; i++)
			{
				if (coll(Char_PosX, Char_PosY, Char_PosX + 20, Char_PosY + 20, p[i].x - 20, p[j].y - 20, p[i].x + 40, p[j].y + 40))
				{
					Rectangle(hmemdc, p[i].x - 40, p[j].y - 40, p[i].x + 40, p[j].y + 40);

				}
				else {
					Rectangle(hmemdc, p[i].x - 30, p[j].y - 30, p[i].x + 30, p[j].y + 30);
				}
			}
		}

		Rectangle(hmemdc, Char_PosX, Char_PosY, Char_PosX + 20, Char_PosY + 20); //주인공 그리기

																				 //   draw on main dc
		BitBlt(hdc, board.left, board.top, board.right, board.bottom, hmemdc, 0, 0, SRCCOPY);

		SelectObject(hmemdc, oldmembitmap); DeleteDC(hmemdc);

		EndPaint(hCWnd, &ps);
		break;

	case WM_DESTROY:
		KillTimer(hCWnd, 1);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hCWnd, iMessage, wParam, lParam);
}

//

bool inRect(const POINT& p, const RECT& r)
{
	if ((r.left <= p.x&&p.x <= r.right) && (r.top <= p.y&&p.y <= r.bottom)) {
		return true;
	}
	else {
		return false;
	}
}

BOOL coll(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
	if (x1 < x4 && x2 > x3 && y2 > y3 && y1 < y4)
	{
		return true;
	}
	return false;
}