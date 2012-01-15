// Application.cpp
// Nolan Check
// Created 1/14/2012

#include "Application.hpp"

Application::Application()
{ }

ApplicationPtr Application::Create(HINSTANCE hInstance, int nShowCmd)
{
	ApplicationPtr p(new Application);

	p->m_window = MainWindow::Create(hInstance, nShowCmd);
	if (!p->m_window) {
		return NULL;
	}

	return p;
}

int Application::MessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}
