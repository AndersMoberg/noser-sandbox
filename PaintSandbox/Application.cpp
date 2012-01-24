// Application.cpp
// Nolan Check
// Created 12/26/2011

#include "Application.hpp"

Application::Application()
{ }

ApplicationPtr Application::Create(HINSTANCE hInstance, int nShowCmd)
{
	ApplicationPtr p(new Application);

	p->m_driver = D3D11::D3D11Driver::Create();
	p->m_canvasWindow = CanvasWindow::Create(p->m_driver, hInstance, nShowCmd);

	return p;
}

int Application::MessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		DispatchMessage(&msg);
		if (m_canvasWindow->IsExceptionThrown()) {
			throw m_canvasWindow->GetExceptionProxy();
		}
	}

	return (int)msg.wParam;
}
