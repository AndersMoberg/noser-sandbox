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
