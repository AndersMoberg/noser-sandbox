// Application.cpp
// Nolan Check
// Created 5/7/2012

#include "Application.hpp"

void Application::init(HINSTANCE hInstance, int nShowCmd)
{
	m_window.init(hInstance, nShowCmd);
	m_renderer.init(m_window.getHWnd());
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
