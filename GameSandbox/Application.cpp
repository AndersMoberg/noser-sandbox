// Application.cpp
// Nolan Check
// Created 3/16/2012

#include "Application.hpp"

Application::Application()
{ }

ApplicationPtr Application::Create(HINSTANCE hInstance, int nShowCmd)
{
	ApplicationPtr p(new Application);

	p->m_window = MainWindow::Create(hInstance, nShowCmd);

	return p;
}

int Application::MessagePump()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		DispatchMessage(&msg);
	}

	if (m_window->IsExceptionThrown()) {
		throw m_window->GetExceptionProxy();
	}

	return (int)msg.wParam;
}
