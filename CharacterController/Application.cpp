// Application.cpp
// Nolan Check
// Created 1/14/2012

#include "Application.hpp"

Application::Application()
{ }

ApplicationPtr Application::Create(HINSTANCE hInstance, int nShowCmd)
{
	ApplicationPtr p(new Application);

	p->m_game = Game::Create();
	p->m_window = MainWindow::Create(hInstance, nShowCmd, p->m_game);

	return p;
}

int Application::MessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		DispatchMessage(&msg);
		if (m_window->ExceptionThrown()) {
			throw m_window->GetExceptionProxy();
		}
	}

	return (int)msg.wParam;
}
