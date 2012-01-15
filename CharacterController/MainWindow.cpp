// MainWindow.cpp
// Nolan Check
// Created 1/15/2012

#include "MainWindow.hpp"

static const LPCTSTR MAINWINDOW_CLASS_NAME =
	TEXT("CharacterControllerWindowClass");

MainWindow::MainWindow()
	: m_hWnd(NULL)
{ }

MainWindow::~MainWindow()
{
	if (m_hWnd != NULL) {
		DestroyWindow(m_hWnd);
	}
}

MainWindowPtr MainWindow::Create(HINSTANCE hInstance, int nShowCmd)
{
	MainWindowPtr p(new MainWindow);

	WNDCLASS wc = { 0 };
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbWndExtra = sizeof(LONG_PTR);
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = MAINWINDOW_CLASS_NAME;
	RegisterClass(&wc);

	CreateWindow(
		MAINWINDOW_CLASS_NAME,
		TEXT("Character Controller"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, nShowCmd,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		p.get());

	return p;
}

LRESULT CALLBACK MainWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result;

	if (uMsg == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		MainWindow* pThis = (MainWindow*)pcs->lpCreateParams;
		SetWindowLongPtr(hwnd, 0, (LONG_PTR)pThis);
		result = pThis->OnWMCreate(hwnd);
	}
	else
	{
		MainWindow* pThis = (MainWindow*)GetWindowLongPtr(hwnd, 0);

		switch (uMsg)
		{
		case WM_DESTROY:
			result = pThis->OnWMDestroy();
			break;
		default:
			result = DefWindowProc(hwnd, uMsg, wParam, lParam);
			break;
		}
	}

	return result;
}

LRESULT MainWindow::OnWMCreate(HWND hwnd)
{
	m_hWnd = hwnd;

	return 0;
}

LRESULT MainWindow::OnWMDestroy()
{
	PostQuitMessage(EXIT_SUCCESS);
	m_hWnd = NULL;
	return 0;
}
