// Application.cpp
// Nolan Check
// Created 5/7/2012

#include "Application.hpp"

void Application::init(HINSTANCE hInstance, int nShowCmd)
{
	m_window = MainWindow::create(hInstance, nShowCmd);
	m_renderer = GLES2Renderer::create(m_window->getHWnd());
}

int Application::messagePump()
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
