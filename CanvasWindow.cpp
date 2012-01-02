// CanvasWindow.cpp
// Nolan Check
// Created 12/26/2011

#include "CanvasWindow.hpp"

static const LPCTSTR CANVASWINDOW_CLASS_NAME = TEXT("CanvasWindowClass");

CanvasWindowPtr CanvasWindow::Create(DriverPtr driver, HINSTANCE hInstance, int nShowCmd)
{
	CanvasWindowPtr result(new CanvasWindow);
	if (!result->CreateInternal(driver, hInstance, nShowCmd)) {
		return NULL;
	}
	return result;
}

CanvasWindow::CanvasWindow()
	: m_hWnd(NULL)
{ }

CanvasWindow::~CanvasWindow()
{
	if (m_hWnd != NULL)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

bool CanvasWindow::CreateInternal(DriverPtr driver, HINSTANCE hInstance, int nShowCmd)
{
	m_driver = driver;

	m_image = CanvasImage::Create(driver, RectF(-10.0f, 10.0f, 10.0f, -10.0f), 2048, 2048);
	if (!m_image) {
		return false;
	}

	RegisterWindowClass(hInstance);

	HWND hWnd = CreateWindow(
		CANVASWINDOW_CLASS_NAME,
		TEXT("Canvas"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, nShowCmd,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		this);
	if (!hWnd) {
		m_hWnd = NULL;
		return false;
	}

	return true;
}

void CanvasWindow::RegisterWindowClass(HINSTANCE hInstance)
{
	WNDCLASS wc = { 0 };
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbWndExtra = sizeof(LONG_PTR);
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = CANVASWINDOW_CLASS_NAME;

	RegisterClass(&wc);
}

LRESULT CALLBACK CanvasWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result;

	if (uMsg == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		CanvasWindow* pThis = (CanvasWindow*)pcs->lpCreateParams;

		SetWindowLongPtr(hwnd, 0, (LONG_PTR)pThis);

		if (!pThis) {
			result = -1;
		} else {
			result = pThis->OnWMCreate(hwnd);
		}
	}
	else
	{
		CanvasWindow* pThis = (CanvasWindow*)GetWindowLongPtr(hwnd, 0);

		switch (uMsg)
		{
		case WM_DESTROY:
			result = pThis->OnWMDestroy();
			break;
		case WM_SIZE:
			result = pThis->OnWMSize();
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

LRESULT CanvasWindow::OnWMCreate(HWND hwnd)
{
	m_hWnd = hwnd;

	m_graphics = m_driver->CreateWindowGraphics(m_hWnd);
	if (!m_graphics) {
		return -1;
	}

	m_graphics->SetCanvasImage(m_image);

	return 0;
}

LRESULT CanvasWindow::OnWMDestroy()
{
	m_graphics.reset();

	PostQuitMessage(EXIT_SUCCESS);
	return 0;
}

LRESULT CanvasWindow::OnWMSize()
{
	m_graphics->OnWMSize();
	return 0;
}

LRESULT CanvasWindow::OnWMPaint()
{
	m_graphics->OnWMPaint();
	return 0;
}
