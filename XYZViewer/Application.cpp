// Application.cpp
// Nolan Check
// Created 5/7/2012

#include "Application.hpp"

Application::Application()
{ }

std::unique_ptr<Application> Application::create(HINSTANCE hInstance, int nShowCmd)
{
	std::unique_ptr<Application> p(new Application);

	p->m_window = MainWindow::create(hInstance, nShowCmd);
	p->m_renderer = GLES2Renderer::create(p->m_window->getHWnd());

	return p;
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
