// MainWindow.cpp
// Nolan Check
// Created 5/7/2012

#include "MainWindow.hpp"

#include <cstdlib>

#include "Application.hpp"

static LPCTSTR MAINWINDOW_CLASS_NAME = TEXT("XYZViewerMainWindowClass");

MainWindow::MainWindow()
	: m_hWnd(NULL)
{ }

MainWindow::~MainWindow()
{
	if (m_hWnd != NULL) {
		DestroyWindow(m_hWnd);
	}
}

std::unique_ptr<MainWindow> MainWindow::create(Application* app, HINSTANCE hInstance, int nShowCmd)
{
	std::unique_ptr<MainWindow> p(new MainWindow);

	p->m_app = app;

	WNDCLASS wc = { 0 };
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbWndExtra = sizeof(LONG_PTR);
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = MAINWINDOW_CLASS_NAME;
	RegisterClass(&wc);

	// The WM_CREATE handler sets m_hWnd
	HWND hWnd = CreateWindow(
		MAINWINDOW_CLASS_NAME,
		TEXT("XYZ Viewer"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, nShowCmd,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		p.get());
	if (!hWnd) {
		throw std::exception("Failed to create main window");
	}

	return p;
}

HWND MainWindow::getHWnd()
{
	return m_hWnd;
}

LRESULT CALLBACK MainWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result;

	if (uMsg == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		MainWindow* self = (MainWindow*)pcs->lpCreateParams;
		SetWindowLongPtr(hwnd, 0, (LONG_PTR)self);

		result = self->onWMCreate(hwnd);
	}
	else
	{
		MainWindow* self = (MainWindow*)GetWindowLongPtr(hwnd, 0);

		switch (uMsg)
		{
		case WM_DESTROY:
			result = self->onWMDestroy();
			break;
		case WM_PAINT:
			result = self->onWMPaint();
			break;
		case WM_KEYDOWN:
			result = self->onWMKeyDown(wParam);
			break;
		default:
			result = DefWindowProc(hwnd, uMsg, wParam, lParam);
			break;
		}
	}

	return result;
}

LRESULT MainWindow::onWMCreate(HWND hwnd)
{
	m_hWnd = hwnd;
	return 0;
}

LRESULT MainWindow::onWMDestroy()
{
	PostQuitMessage(EXIT_SUCCESS);
	m_hWnd = NULL;
	return 0;
}

LRESULT MainWindow::onWMPaint()
{
	m_app->paint();
	ValidateRect(m_hWnd, NULL);
	return 0;
}

LRESULT MainWindow::onWMKeyDown(WPARAM wParam)
{
	switch (wParam)
	{
	case VK_LEFT:
		m_app->onLeft();
		break;
	case VK_RIGHT:
		m_app->onRight();
		break;
	case VK_UP:
		m_app->onUp();
		break;
	case VK_DOWN:
		m_app->onDown();
		break;
	}

	return 0;
}
