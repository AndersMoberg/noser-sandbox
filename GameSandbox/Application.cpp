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

void Application::init(HINSTANCE hInstance, int nShowCmd)
{
	timeBeginPeriod(1);

	CHECK_HR(CoInitialize(NULL));
	m_coInited = true;

	m_window.init(hInstance, nShowCmd);
}

int Application::MessagePump()
{
	MSG msg;

	for (;;)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) {
				break;
			} else {
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (m_window.IsExceptionThrown()) {
				throw m_window.GetExceptionProxy();
			}
			m_window.process();
		}
	}

	if (m_window.IsExceptionThrown()) {
		throw m_window.GetExceptionProxy();
	}

	return (int)msg.wParam;
}
