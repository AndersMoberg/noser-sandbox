// MainWindow.cpp
// Nolan Check
// Created 3/16/2012

#include "MainWindow.hpp"

static LPCTSTR MAINWINDOW_CLASS_NAME = TEXT("GameSandboxMainWindowClass");

MainWindow::MainWindow()
	: m_hWnd(NULL),
	m_exceptionThrown(false),
	m_leftToRightKeys(0),
	m_downToUpKeys(0)
{ }

MainWindow::~MainWindow()
{
	if (m_hWnd != NULL) {
		DestroyWindow(m_hWnd);
	}
}

MainWindow* MainWindow::Create(HINSTANCE hInstance, int nShowCmd)
{
	MainWindow* p(new MainWindow);

	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	p->m_frequency = frequency.QuadPart;

	LARGE_INTEGER curTime;
	QueryPerformanceCounter(&curTime);
	p->m_curTime = curTime.QuadPart;

	WNDCLASS wc = { 0 };
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbWndExtra = sizeof(LONG_PTR);
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = MAINWINDOW_CLASS_NAME;
	RegisterClass(&wc);

	HWND hWnd = CreateWindow(
		MAINWINDOW_CLASS_NAME,
		TEXT("Game Sandbox"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, nShowCmd,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		p);
	// The WM_CREATE handler sets m_hWnd
	if (!hWnd)
	{
		p->m_hWnd = NULL;
		if (p->m_exceptionThrown) {
			throw p->m_exceptionProxy;
		} else {
			throw std::exception("Failed to create window");
		}
	}

	return p;
}

LRESULT CALLBACK MainWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result;

	if (uMsg == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		MainWindow* self = (MainWindow*)pcs->lpCreateParams;
		SetWindowLongPtr(hwnd, 0, (LONG_PTR)self);

		try
		{
			result = self->OnWMCreate(hwnd);
		}
		catch (const std::exception& e)
		{
			self->m_exceptionThrown = true;
			self->m_exceptionProxy = e;
			result = -1;
		}
	}
	else
	{
		MainWindow* self = (MainWindow*)GetWindowLongPtr(hwnd, 0);

		try
		{
			switch (uMsg)
			{
			case WM_DESTROY:
				result = self->OnWMDestroy();
				break;
			case WM_SIZE:
				result = self->OnWMSize();
				break;
			case WM_PAINT:
				result = self->OnWMPaint();
				break;
			case WM_KEYDOWN:
				result = self->OnWMKeyDown(wParam);
				break;
			case WM_KEYUP:
				result = self->OnWMKeyUp(wParam);
				break;
			default:
				result = DefWindowProc(hwnd, uMsg, wParam, lParam);
				break;
			}
		}
		catch (const std::exception& e)
		{
			self->m_exceptionThrown = true;
			self->m_exceptionProxy = e;
			result = 0;
		}
	}

	return result;
}

LRESULT MainWindow::OnWMCreate(HWND hWnd)
{
	m_hWnd = hWnd;

	m_game.reset(Game::Create(m_hWnd));

	return 0;
}

LRESULT MainWindow::OnWMDestroy()
{
	m_game.reset();

	PostQuitMessage(EXIT_SUCCESS);

	m_hWnd = NULL;
	return 0;
}

LRESULT MainWindow::OnWMSize()
{
	m_game->Resize();
	return 0;
}

LRESULT MainWindow::OnWMPaint()
{
	// FIXME: WM_PAINT is not the ideal place to call Update.
	Update();

	Render();
	Present();

	// Animate!
	InvalidateRect(m_hWnd, NULL, FALSE);

	return 0;
}

LRESULT MainWindow::OnWMKeyDown(WPARAM wParam)
{
	switch (wParam)
	{
	case VK_LEFT:
		m_leftToRightKeys = -1;
		break;
	case VK_UP:
		m_downToUpKeys = 1;
		break;
	case VK_RIGHT:
		m_leftToRightKeys = 1;
		break;
	case VK_DOWN:
		m_downToUpKeys = -1;
		break;
	}
	return 0;
}

LRESULT MainWindow::OnWMKeyUp(WPARAM wParam)
{
	switch (wParam)
	{
	case VK_LEFT:
		m_leftToRightKeys = (GetKeyState(VK_RIGHT) & 0x8000) ? 1 : 0;
		break;
	case VK_UP:
		m_downToUpKeys = (GetKeyState(VK_DOWN) & 0x8000) ? -1 : 0;
		break;
	case VK_RIGHT:
		m_leftToRightKeys = (GetKeyState(VK_LEFT) & 0x8000) ? -1 : 0;
		break;
	case VK_DOWN:
		m_downToUpKeys = (GetKeyState(VK_UP) & 0x8000) ? 1 : 0;
		break;
	}
	return 0;
}

static const float SQRT_1_OVER_2 = 0.70710677f;

void MainWindow::Update()
{
	// Gather input
	Vector2f move(0.0f, 0.0f);
	if (m_leftToRightKeys == -1 && m_downToUpKeys == -1) {
		move = Vector2f(-SQRT_1_OVER_2, -SQRT_1_OVER_2); // Southwest
	} else if (m_leftToRightKeys == -1 && m_downToUpKeys == 0) {
		move = Vector2f(-1.0f, 0.0f); // West
	} else if (m_leftToRightKeys == -1 && m_downToUpKeys == 1) {
		move = Vector2f(-SQRT_1_OVER_2, SQRT_1_OVER_2); // Northwest
	} else if (m_leftToRightKeys == 0 && m_downToUpKeys == -1) {
		move = Vector2f(0.0f, -1.0f); // South
	} else if (m_leftToRightKeys == 0 && m_downToUpKeys == 0) {
		move = Vector2f(0.0f, 0.0f); // Center
	} else if (m_leftToRightKeys == 0 && m_downToUpKeys == 1) {
		move = Vector2f(0.0f, 1.0f); // North
	} else if (m_leftToRightKeys == 1 && m_downToUpKeys == -1) {
		move = Vector2f(SQRT_1_OVER_2, -SQRT_1_OVER_2); // Southeast
	} else if (m_leftToRightKeys == 1 && m_downToUpKeys == 0) {
		move = Vector2f(1.0f, 0.0f); // East
	} else if (m_leftToRightKeys == 1 && m_downToUpKeys == 1) {
		move = Vector2f(SQRT_1_OVER_2, SQRT_1_OVER_2); // Northeast
	}

	LARGE_INTEGER liCurTime;
	QueryPerformanceCounter(&liCurTime);

	long long tickTime = m_frequency / (long long)Game::TICKS_PER_SEC;

	GameInput input;
	input.move = move;
	input.enter = !!(GetKeyState(VK_RETURN) & 0x8000);
	input.esc = !!(GetKeyState(VK_ESCAPE) & 0x8000);

	long long t;
	for (t = m_curTime; t < liCurTime.QuadPart; t += tickTime)
	{
		m_game->Tick(input);

		input.enter = false;
		input.esc = false;
	}

	m_curTime = t;
}

void MainWindow::Render()
{
	m_game->Render();
}

void MainWindow::Present()
{
	m_game->Present();
}
