// MainWindow.cpp
// Nolan Check
// Created 1/15/2012

#include "MainWindow.hpp"

#include <WindowsX.h>

#include <GLES2/gl2.h>

#include "WindowsUtils.hpp"

static const LPCTSTR MAINWINDOW_CLASS_NAME =
	TEXT("CharacterControllerWindowClass");

MainWindow::MainWindow()
	: m_exceptionThrown(false),
	m_hWnd(NULL),
	m_bgTexture(0),
	m_leftToRightKeys(0),
	m_downToUpKeys(0),
	m_spaceTrigger(false)
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

	p->m_game = Game::Create();

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
		MainWindow* pThis = (MainWindow*)pcs->lpCreateParams;
		SetWindowLongPtr(hwnd, 0, (LONG_PTR)pThis);

		result = -1;
		try
		{
			result = pThis->OnWMCreate(hwnd);
		}
		catch (const std::exception& e)
		{
			pThis->m_exceptionThrown = true;
			pThis->m_exceptionProxy = e;
			result = -1;
		}
	}
	else
	{
		MainWindow* pThis = (MainWindow*)GetWindowLongPtr(hwnd, 0);

		result = 0;
		try
		{
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
			case WM_KEYDOWN:
				result = pThis->OnWMKeyDown(wParam);
				break;
			case WM_KEYUP:
				result = pThis->OnWMKeyUp(wParam);
				break;
			default:
				result = DefWindowProc(hwnd, uMsg, wParam, lParam);
				break;
			}
		}
		catch (const std::exception& e)
		{
			pThis->m_exceptionThrown = true;
			pThis->m_exceptionProxy = e;
			result = 0;
		}
	}

	return result;
}

LRESULT MainWindow::OnWMCreate(HWND hwnd)
{
	m_hWnd = hwnd;

	m_gles2Manager = GLES2Manager::Create(m_hWnd);
	m_d2dLayer = D2DLayer::Create(m_hWnd);

	m_bgTexture = m_gles2Manager->CreateTextureFromFile(L"C:\\Users\\Public\\Pictures\\Sample Pictures\\Tulips.jpg");

	CreateDeviceResources();

	return 0;
}

LRESULT MainWindow::OnWMDestroy()
{
	m_bgTexture.reset();
	DestroyDeviceResources();
	m_d2dLayer.reset();
	m_gles2Manager.reset();

	PostQuitMessage(EXIT_SUCCESS);

	m_hWnd = NULL;
	return 0;
}

LRESULT MainWindow::OnWMSize(LPARAM lParam)
{
	D2D1_SIZE_U size = D2D1::SizeU(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	m_d2dLayer->Resize(size);

	return 0;
}

LRESULT MainWindow::OnWMPaint()
{
	// FIXME: This is not the right place to call update
	Update();

	Render();

	RECT clientRc;
	GetClientRect(m_hWnd, &clientRc);
	glViewport(0, 0, clientRc.right - clientRc.left, clientRc.bottom - clientRc.top);

	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	// Draw background image
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_bgTexture->Get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Premultiplied alpha blending
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	glEnable(GL_BLEND);

	m_gles2Manager->DrawTexturedQuad(Rectf(-1.0f, 1.0f, 1.0f, -1.0f));

	// Draw Direct2D overlay
	GLuint d2dTexture = m_d2dLayer->GetGLTexture();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, d2dTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Premultiplied alpha blending
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	glEnable(GL_BLEND);

	m_gles2Manager->DrawTexturedQuad(Rectf(-1.0f, 1.0f, 1.0f, -1.0f));

	m_gles2Manager->Present();

	// Cause animation
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
	case VK_SPACE:
		m_spaceTrigger = true;
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

void MainWindow::CreateDeviceResources()
{
	m_d2dLayer->CreateDeviceResources();
	m_game->SetD2DTarget(m_d2dLayer->GetD2DTarget());
}

void MainWindow::DestroyDeviceResources()
{
	m_game->ReleaseD2DTarget();
	m_d2dLayer->DestroyDeviceResources();
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

	m_game->Update(move, m_spaceTrigger);
	m_spaceTrigger = false;

	InvalidateRect(m_hWnd, NULL, FALSE);
}

void MainWindow::Render()
{
	CreateDeviceResources();

	ComPtr<ID2D1RenderTarget> d2dTarget = m_d2dLayer->GetD2DTarget();
	d2dTarget->BeginDraw();

	m_game->Render();

	HRESULT hr = d2dTarget->EndDraw();
	if (hr == D2DERR_RECREATE_TARGET)
	{
		// Target will be recreated on the next call to Render
		DestroyDeviceResources();
	}
	else
	{
		CHECK_HR(hr);
	}
}
