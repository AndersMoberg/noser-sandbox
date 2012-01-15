// MainWindow.cpp
// Nolan Check
// Created 1/15/2012

#include "MainWindow.hpp"

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

MainWindowPtr MainWindow::Create(HINSTANCE hInstance, int nShowCmd)
{
	MainWindowPtr p(new MainWindow);

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
