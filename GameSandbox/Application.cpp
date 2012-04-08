// Application.cpp
// Nolan Check
// Created 3/16/2012

#include "Application.hpp"

#include <ObjBase.h>
#include <MMSystem.h>

#include "WindowsUtils.hpp"

Application::Application()
	: m_coInited(false)
{ }

Application::~Application()
{
	if (m_coInited)
	{
		CoUninitialize();
		m_coInited = false;
	}

	timeEndPeriod(1);
}

Application* Application::Create(HINSTANCE hInstance, int nShowCmd)
{
	Application* p(new Application);

	timeBeginPeriod(1);

	CHECK_HR(CoInitialize(NULL));
	p->m_coInited = true;

	p->m_window.reset(MainWindow::Create(hInstance, nShowCmd));

	return p;
}

int Application::MessagePump()
{
	MSG msg;

	for (;;)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) {
				goto quit;
			} else {
				DispatchMessage(&msg);
			}
		}

		m_window->process();
	}
	quit:

	if (m_window->IsExceptionThrown()) {
		throw m_window->GetExceptionProxy();
	}

	return (int)msg.wParam;
}
