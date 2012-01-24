// Main.cpp
// Nolan Check
// Created 12/26/2011

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <tchar.h>

#include "Application.hpp"

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{
	int exitCode = EXIT_FAILURE;

	try
	{
		ApplicationPtr app = Application::Create(hInstance, nShowCmd);
		exitCode = app->MessageLoop();
	}
	catch (const std::exception& e)
	{
		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");
		exitCode = EXIT_FAILURE;
	}

	return exitCode;
}
