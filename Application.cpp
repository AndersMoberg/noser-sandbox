// Application.cpp
// Nolan Check
// Created 12/26/2011

#include "Application.hpp"

ApplicationPtr Application::Create(HINSTANCE hInstance, int nShowCmd)
{
	ApplicationPtr result(new Application);
	if (!result->CreateInternal(hInstance, nShowCmd)) {
		return NULL;
	}
	return result;
}

Application::Application()
{ }

bool Application::CreateInternal(HINSTANCE hInstance, int nShowCmd)
{
	m_driver = D3D11::D3D11Driver::Create();
	if (!m_driver) {
		return false;
	}

	m_canvasWindow = CanvasWindow::Create(m_driver, hInstance, nShowCmd);
	if (!m_canvasWindow) {
		return false;
	}

	return true;
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
