// CanvasWindow.cpp
// Nolan Check
// Created 12/26/2011

#include "CanvasWindow.hpp"

#include <WindowsX.h>

#include "D3D11/D3D11Driver.hpp"
#include "D3D11/D3D11Image.hpp"

static const LPCTSTR CANVASWINDOW_CLASS_NAME = TEXT("CanvasWindowClass");

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

CanvasWindowPtr CanvasWindow::Create(DriverPtr driver, HINSTANCE hInstance, int nShowCmd)
{
	CanvasWindowPtr p(new CanvasWindow);

	p->m_driver = driver;

	p->m_image = CanvasImage::Create(driver, RectF(-10.0f, 10.0f, 10.0f, -10.0f), 2048, 2048);
	if (!p->m_image) {
		return NULL;
	}

	p->m_drawTool = DrawTool::Create(p->m_driver, p->m_image);
	if (!p->m_drawTool) {
		return NULL;
	}

	p->RegisterWindowClass(hInstance);

	HWND hWnd = CreateWindow(
		CANVASWINDOW_CLASS_NAME,
		TEXT("Canvas"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, nShowCmd,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		p.get());
	if (!hWnd) {
		p->m_hWnd = NULL;
		return NULL;
	}

	return p;
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
		case WM_MOUSEMOVE:
			result = pThis->OnWMMouseMove(wParam, lParam);
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

LRESULT CanvasWindow::OnWMMouseMove(WPARAM wParam, LPARAM lParam)
{
	if (wParam & MK_LBUTTON)
	{
		// Add 0.5 because the cursor is in the center of the pixel.
		float x = GET_X_LPARAM(lParam) + 0.5f;
		float y = GET_Y_LPARAM(lParam) + 0.5f;
		Vector2f pos(x, y);

		RECT clientRc;
		GetClientRect(m_hWnd, &clientRc);
		RectF clientRect((float)clientRc.left, (float)clientRc.top,
			(float)clientRc.right, (float)clientRc.bottom);

		Matrix3x2f clientToCanvas = Matrix3x2f::RectLerp(
			clientRect, m_image->GetCanvasRect());

		Vector2f canvasPos = clientToCanvas.TransformPoint(pos);
		m_drawTool->ReceiveCursor(canvasPos);

		InvalidateRect(m_hWnd, NULL, FALSE);
	}

	return 0;
}
