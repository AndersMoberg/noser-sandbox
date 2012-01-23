// MainWindow.cpp
// Nolan Check
// Created 1/15/2012

#include "MainWindow.hpp"

#include <WindowsX.h>

#include <vector>

static const LPCTSTR MAINWINDOW_CLASS_NAME =
	TEXT("CharacterControllerWindowClass");

template<class Interface>
inline void SafeRelease(Interface*& pInterface)
{
	if (pInterface)
	{
		pInterface->Release();
		pInterface = NULL;
	}
}

MainWindow::MainWindow()
	: m_hWnd(NULL),
	m_pD2DFactory(NULL),
	m_pD2DTarget(NULL)
{ }

MainWindow::~MainWindow()
{
	if (m_hWnd != NULL) {
		DestroyWindow(m_hWnd);
	}

	DestroyDeviceResources();
	SafeRelease(m_pD2DFactory);
}

MainWindowPtr MainWindow::Create(HINSTANCE hInstance, int nShowCmd, GamePtr game)
{
	MainWindowPtr p(new MainWindow);

	p->m_game = game;

	HRESULT hr;

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &p->m_pD2DFactory);
	if (FAILED(hr)) {
		return NULL;
	}

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
		TEXT("Character Controller"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, nShowCmd,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		p.get());
	// The WM_CREATE handler sets m_hWnd
	if (!hWnd)
	{
		p->m_hWnd = NULL;
		return NULL;
	}

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
		case WM_SIZE:
			result = pThis->OnWMSize(lParam);
			break;
		case WM_PAINT:
			result = pThis->OnWMPaint();
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

	if (!CreateDeviceResources()) {
		return -1;
	}

	return 0;
}

LRESULT MainWindow::OnWMDestroy()
{
	DestroyDeviceResources();

	PostQuitMessage(EXIT_SUCCESS);

	m_hWnd = NULL;
	return 0;
}

LRESULT MainWindow::OnWMSize(LPARAM lParam)
{
	if (m_pD2DTarget)
	{
		D2D1_SIZE_U size = D2D1::SizeU(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		m_pD2DTarget->Resize(size);
	}

	return 0;
}

LRESULT MainWindow::OnWMPaint()
{
	// BeginPaint..EndPaint are required to validate the rectangle so Windows
	// doesn't send WM_PAINT incessantly
	PAINTSTRUCT ps;
	BeginPaint(m_hWnd, &ps);

	// TODO: This is the wrong place to call Update.
	Update();
	Render();

	// Trigger animation by invalidating here
	InvalidateRect(m_hWnd, NULL, FALSE);

	EndPaint(m_hWnd, &ps);
	return 0;
}

bool MainWindow::CreateDeviceResources()
{
	if (!m_pD2DTarget)
	{
		RECT rc;
		GetClientRect(m_hWnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

		HRESULT hr = m_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hWnd, size),
			&m_pD2DTarget);
		if (FAILED(hr)) {
			return false;
		}
	}

	return true;
}

void MainWindow::DestroyDeviceResources()
{
	SafeRelease(m_pD2DTarget);
}

void MainWindow::Update()
{
	// Gather input
	Vector2f move(0.5f, 0.0f);

	m_game->Update(move);
}

void MainWindow::Render()
{
	CreateDeviceResources();

	m_pD2DTarget->BeginDraw();

	m_game->Render(m_pD2DTarget);

	HRESULT hr = m_pD2DTarget->EndDraw();
	if (hr == D2DERR_RECREATE_TARGET)
	{
		// Target will be recreated on the next call to Render
		DestroyDeviceResources();
	}
	// TODO: Better handling of other D2D errors.
}
