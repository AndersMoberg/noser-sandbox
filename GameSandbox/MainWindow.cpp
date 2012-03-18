// MainWindow.cpp
// Nolan Check
// Created 3/16/2012

#include "MainWindow.hpp"

static LPCTSTR MAINWINDOW_CLASS_NAME = TEXT("GameSandboxMainWindowClass");

MainWindow::MainWindow()
	: m_hWnd(NULL),
	m_exceptionThrown(false)
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

	HWND hWnd = CreateWindow(
		MAINWINDOW_CLASS_NAME,
		TEXT("Game Sandbox"),
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
			case WM_PAINT:
				result = self->OnWMPaint();
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

	m_renderer = GameRenderer::Create(m_hWnd);
	m_game = Game::Create(m_renderer);

	m_bgTexture = m_renderer->GetGLES2Manager()->CreateTextureFromFile(L"C:\\Users\\Public\\Pictures\\Sample Pictures\\Tulips.jpg");

	return 0;
}

LRESULT MainWindow::OnWMDestroy()
{
	m_bgTexture.reset();
	m_game.reset();
	m_renderer.reset();

	PostQuitMessage(EXIT_SUCCESS);

	m_hWnd = NULL;
	return 0;
}

LRESULT MainWindow::OnWMPaint()
{
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

	m_renderer->GetGLES2Manager()->DrawTexturedQuad(Rectf(-1.0f, 1.0f, 1.0f, -1.0f));

	m_renderer->GetGLES2Manager()->Present();

	ValidateRect(m_hWnd, NULL);
	return 0;
}
