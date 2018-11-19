#include <tchar.h>
#include <random>
#include <iostream>

#include "MyVector.h"
#include "GameManager.h"

HWND g_hParent;		//	패런트 윈도우 핸들 글로벌
HWND g_hChild;		//	차일드 윈도우 핸들 글로벌
HINSTANCE g_hInst;	//	인스턴스 핸들 글로벌
LPCTSTR lpszClass1 = "ParentClass";
LPCTSTR lpszClass2 = "ChildClass";

// 사용자 정의 구조체들 
LogoImage li;
BackGround bg;
Enemy e;
GameManager gm;
Cannon c;
Character ch;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ChildProc(HWND hCWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

//	사용자 정의 함수들 TEST 용	//////////////
/* 여기다가 대충 끄적여 놓고 옮겨놓기 */

//////////////////////////////////////////////

FMOD_SYSTEM* soundSystem;
FMOD_SOUND *soundFile;
FMOD_CHANNEL *channel

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{

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

	//	자식
	WndClass.hCursor = LoadCursor(NULL, IDC_CROSS);
	//	커서 없애줘야함.
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszClassName = lpszClass2;
	WndClass.lpfnWndProc = (WNDPROC)ChildProc;

	RegisterClassEx(&WndClass);

	g_hParent = CreateWindow(
		lpszClass1,
		"TEST PROGRAM",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_BORDER,
		0, 0,
		WindowWidth, WindowHeight,
		NULL,
		(HMENU)NULL,
		hInstance,
		NULL
	);
	ShowWindow(g_hParent, nCmdShow);
	UpdateWindow(g_hParent);

	while (TRUE) {
		std::chrono::system_clock::time_point currTime = std::chrono::system_clock::now();
		if (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
		{
			if (Message.message == WM_QUIT) break;
			TranslateMessage(&Message);			// 메시지 이동
			DispatchMessage(&Message);			// 메시지 파견
		}
		else {
			
			gm.elapsedTime = std::chrono::system_clock::now() - gm.startTime;
			gm.frameTime = std::chrono::system_clock::now() - currTime;
		}
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	FMOD_System_Create(&soundSystem);
	FMOD_System_Init(soundSystem, 10, FMOD_INIT_NORMAL, NULL);
	FMOD_System_CreateSound(soundSystem, "Sound.mp3", FMOD_LOOP_NORMAL, 0, &soundFile);
	FMOD_System_PlaySound(soundSystem, soundFile, NULL, 0, &channel);

	PAINTSTRUCT ps;
	HDC hdc, hmemdc;
	static HBITMAP hmembitmap, oldmembitmap;
	static RECT board;	//	실제 보드

	switch (iMessage)
	{
	case WM_CREATE:
		GetClientRect(hWnd, &board);
		g_hChild = CreateWindow("ChildClass", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
			board.right / 2 - ChildWindowSize / 2, board.bottom / 2 - ChildWindowSize / 2, ChildWindowSize, ChildWindowSize, hWnd, NULL, g_hInst, NULL);
		SetTimer(g_hChild, 4, 17, NULL);
		ShowCursor(false);
		break;

	case WM_KEYUP:
		if (gm.isStarted) {
			switch (wParam)
			{
			case VK_LEFT:
				ch.bm.isLeft = false;
				break;
			case VK_RIGHT:
				ch.bm.isRight = false;
				break;
			case VK_UP:
				ch.bm.isUp = false;
				break;
			case VK_DOWN:
				ch.bm.isDown = false;
				break;
			default:
				break;
			}
		}
		break;

	case WM_KEYDOWN:
		/* 부모 윈도우에서만 키다운 처리 가능, 그래서 글로벌 차일드 윈도우 핸들을 인자로 넘겨주어 invalidateRect 실시
		만약 부모 윈도우꺼 주면 깜빡임 현상 발생했음. *의문인건 부모 윈도우도 더블버퍼링이 적용되었는데. */
		if (gm.isStarted) {
			switch (wParam) {

			case VK_LEFT:
				ch.bm.isRight = false;
				ch.bm.isLeft = true;
				break;
			case VK_RIGHT:
				ch.bm.isLeft = false;
				ch.bm.isRight = true;
				break;
			case VK_UP:
				ch.bm.isDown = false;
				ch.bm.isUp = true;
				break;
			case VK_DOWN:
				ch.bm.isUp = false;
				ch.bm.isDown = true;
				break;
			default:
				break;
			}
		}
		else {
			if (wParam == VK_SPACE) {
				/* 게임 스타트 키, 스페이스 바 */
				gm.startTime = std::chrono::system_clock::now();
				if (!gm.isStarted) gm.isStarted = true;
				SetTimer(g_hChild, 2, 2000, NULL);
				KillTimer(g_hChild, 3);
			}
			else if (wParam == VK_ESCAPE) {
				/* 게임이 실행 중이던 아니던 ESC키 누르면 윈도우 창 끄기 */
				PostQuitMessage(0);
			}
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		if (hmembitmap == NULL) {
			hmembitmap = CreateCompatibleBitmap(hdc, board.right, board.bottom);
		}
		hmemdc = CreateCompatibleDC(hdc);		//	메인버퍼 호환되는 백버퍼
		oldmembitmap = (HBITMAP)SelectObject(hmemdc, (HBITMAP)hmembitmap);
		//	draw background
		Rectangle(hmemdc, board.left, board.top, board.right, board.bottom);
		//	draw on main dc
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

	switch (iMessage)
	{
	case WM_CREATE:
	{
		/* 초기화 */
		GetClientRect(hCWnd, &gm.board);
		c.InitiateCannon(gm);
		ch.Initiate(gm);
	}
	{
		/* 배경 사각형의 중점 좌표 */
		for (int i = 0; i < 8; ++i) {
			bg.p[i].x = gm.board.right / 2 - (10 + 20 * 3 + 210) + BGSize * i;
			bg.p[i].y = gm.board.bottom / 2 - (10 + 20 * 3 + 210) + BGSize * i;
			bg.r[i].left = bg.p[i].x - BGSize / 2;
			bg.r[i].top = bg.p[i].y - BGSize / 2;
			bg.r[i].right = bg.p[i].x + BGSize / 2;
			bg.r[i].bottom = bg.p[i].y + BGSize / 2;
		}
	}

	{
		/* 이미지 로드부 */
		c.img.Load(TEXT("Resource\\Cannon.png"));
		li.logo.Load(TEXT("Resource\\Logo.png"));
		li.keyboard.Load(TEXT("Resource\\Keyboard.png"));
	}

	{ /* 타이머 셋팅부*/
		SetTimer(hCWnd, 3, 100, NULL);
	}
		break;

	case WM_TIMER:
		switch (wParam)
		{
		case 1:
		{
			/* 대포의 각도 증가 타이머 */
			if (c.currDegree != c.targetDegree) {
				if (-1 < c.currDegree&&c.currDegree < 361) {
					if (dotproduct(c.currV, c.targetV) >= 0) {
						c.currDegree += 5;
					}
					else {
						c.currDegree -= 5;
					}
				}
				else if (c.currDegree > 360) {
					c.currDegree = 0;
				}
				else if (c.currDegree < 0) {
					c.currDegree = 360;
				}
			}
			else {
				KillTimer(hCWnd, 1);
				SetTimer(hCWnd, 2, 2000, NULL);
			}
		}
		break;

		case 2:
		{
			/* 랜덤 생성 각도
			*/
			std::random_device rd;
			std::default_random_engine dre(rd());
			std::uniform_int_distribution<> uidAngle(1, 8);
			switch (uidAngle(dre)) {
			case 1:
				c.targetDegree = 45 * 0;
				break;
			case 2:
				c.targetDegree = 45 * 1;
				break;
			case 3:
				c.targetDegree = 45 * 2;
				break;
			case 4:
				c.targetDegree = 45 * 3;
				break;
			case 5:
				c.targetDegree = 45 * 4;
				break;
			case 6:
				c.targetDegree = 45 * 5;
				break;
			case 7:
				c.targetDegree = 45 * 6;
				break;
			case 8:
				c.targetDegree = 45 * 7;
				break;
			}
			c.setVector(c.targetV, gm, c.targetDegree);
			c.setVector(c.currV, gm, c.currDegree);

			/* 여기에 적 생성 및 적 벡터를 타겟벡터로 셋팅? */
			KillTimer(hCWnd, 2);
			SetTimer(hCWnd, 1, 17, NULL);
		}
		break;

		case 3:
		{
			/* 로고 이미지 프레임 주기 */
			li.frame++;
			if (li.frame == 3) {
				li.frame = 0;
			}
		}
		break;

		case 4:
		{
			/* 이미지 드로우 (17ms마다 그림 1장씩 출력하도록, frame 효과를 최대한 흉내내고 싶었음 
			그냥 프레임타임으로 짜고 타이머는 전혀 쓰지말껄.. 후회도됨 */
			InvalidateRect(g_hChild, NULL, false);
		}
			break;
		}
		break;

	case WM_PAINT:
		if (gm.isStarted) {
			gm.hdc = BeginPaint(hCWnd, &ps);

			if (gm.hmembit == NULL) {
				gm.hmembit = CreateCompatibleBitmap(gm.hdc, gm.board.right, gm.board.bottom);
			}

			gm.hmemdc = CreateCompatibleDC(gm.hdc);									//	메인버퍼 호환되는 백버퍼
			gm.oldmembit = (HBITMAP)SelectObject(gm.hmemdc, (HBITMAP)gm.hmembit);	//	백버퍼 이미지

			//	background draw
			gm.hbrush = (HBRUSH)CreateSolidBrush(RGB(214, 230, 240));
			gm.oldbrush = (HBRUSH)SelectObject(gm.hmemdc, gm.hbrush);
			Rectangle(gm.hmemdc, gm.board.left, gm.board.top, gm.board.right, gm.board.bottom);
			SelectObject(gm.hmemdc, gm.oldbrush); DeleteObject(gm.hbrush);

			//	background rects draw
			{
				gm.hbrush = (HBRUSH)CreateHatchBrush(HS_DIAGCROSS, RGB(50, 96, 155));
				gm.oldbrush = (HBRUSH)SelectObject(gm.hmemdc, gm.hbrush);
				gm.hpen = (HPEN)CreatePen(PS_SOLID, 1, RGB(214, 230, 240));
				gm.oldpen = (HPEN)SelectObject(gm.hmemdc, gm.hpen); 
				for (int y = 0; y < 8; ++y) {
					for (int x = 0; x < 8; ++x) {
						if (inRect(ch.loc, bg.r[x], bg.r[y])) {
							Rectangle(gm.hmemdc, bg.r[x].left, bg.r[y].top, bg.r[x].right, bg.r[y].bottom);
						}
						else {
							Rectangle(gm.hmemdc, bg.p[x].x - 30, bg.p[y].y - 30, bg.p[x].x + 30, bg.p[y].y + 30);
						}
					}
				}
				SelectObject(gm.hmemdc, gm.oldbrush); DeleteObject(gm.hbrush);
				SelectObject(gm.hmemdc, gm.oldpen); DeleteObject(gm.hpen);
			}

			//	3 좌표 구하기 for plgblt
			getCannonPointRotated(c.loc, c.currDegree, gm.board, CLOCKWISE);
			

			//	cannon draw
			c.img.PlgBlt(gm.hmemdc, c.loc);

			//	character draw
			ch.doMove(gm);
			ch.doRotate(gm);
			getPointRotate(ch.rect, ch.degree, ch.loc);
			ch.draw(gm);

			if (0) {
				for (int j = 0; j < gm.board.bottom; ++j) {
					for (int i = 0; i < gm.board.right; ++i) {
						COLORREF temp = GetPixel(gm.hmemdc, i, j);
						SetPixel(gm.hmemdc, i, j, makeReverseRGB(temp));
					}
				}
			}

			//	draw on main dc
			BitBlt(gm.hdc, gm.board.left, gm.board.top, gm.board.right, gm.board.bottom, gm.hmemdc, 0, 0, SRCCOPY);
			SelectObject(gm.hmemdc, gm.oldmembit); DeleteDC(gm.hmemdc);
			EndPaint(hCWnd, &ps);
			
		}
		else {
			gm.hdc = BeginPaint(hCWnd, &ps);
			if (gm.hmembit == NULL) {
				gm.hmembit = CreateCompatibleBitmap(gm.hdc, gm.board.right, gm.board.bottom);
			}
			gm.hmemdc = CreateCompatibleDC(gm.hdc);									//	메인버퍼 호환되는 백버퍼
			gm.oldmembit = (HBITMAP)SelectObject(gm.hmemdc, (HBITMAP)gm.hmembit);	//	백버퍼 이미지
			li.logo.Draw(gm.hmemdc, gm.board.left, gm.board.top, gm.board.right, gm.board.bottom, li.frame * LgSize, 0, LgSize, LgSize);
			li.keyboard.Draw(gm.hmemdc, 64, 604, 132, 88, li.frame * KbWidth, 0, KbWidth, KbHeight);
			//	draw on main dc
			BitBlt(gm.hdc, gm.board.left, gm.board.top, gm.board.right, gm.board.bottom, gm.hmemdc, 0, 0, SRCCOPY);
			SelectObject(gm.hmemdc, gm.oldmembit); DeleteDC(gm.hmemdc);
			EndPaint(hCWnd, &ps);
		}
		break;

	case WM_DESTROY:
		li.keyboard.Destroy();
		li.logo.Destroy();
		c.img.Destroy();
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hCWnd, iMessage, wParam, lParam);
}