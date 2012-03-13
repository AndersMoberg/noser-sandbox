// Application.cpp
// Nolan Check
// Created 1/14/2012

#include "Application.hpp"

#include "WindowsUtils.hpp"

Application::Application()
{ }

Application::~Application()
{
	CoUninitialize();
}

ApplicationPtr Application::Create(HINSTANCE hInstance, int nShowCmd)
{
	ApplicationPtr p(new Application);

	CHECK_HR(CoInitialize(NULL));

	p->m_game = Game::Create();
	p->m_window = MainWindow::Create(hInstance, nShowCmd, p->m_game);

	return p;
}

int Application::MessageLoop()
{
	MSG msg;

	for (;;)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) {
				break;
			}

			DispatchMessage(&msg);
			if (m_window->ExceptionThrown()) {
				throw m_window->GetExceptionProxy();
			}
		}
		else
		{
			m_window->Update();
		}
	}

	return (int)msg.wParam;
}
