// Application.cpp
// Nolan Check
// Created 5/7/2012

#include "Application.hpp"

#include <GLES2/gl2.h>

Application::Application()
{ }

std::unique_ptr<Application> Application::create(HINSTANCE hInstance, int nShowCmd)
{
	std::unique_ptr<Application> p(new Application);

	p->m_window = MainWindow::create(p.get(), hInstance, nShowCmd);
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

void Application::paint()
{
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, m_renderer->getWidth(), m_renderer->getHeight());

	m_renderer->present();
}
