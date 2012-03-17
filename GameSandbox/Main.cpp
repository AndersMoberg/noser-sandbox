// Main.cpp
// Nolan Check
// Created 3/16/2012

#if _DEBUG
// Memory leak checker
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <cstdlib>
#include <exception>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <tchar.h>

#include "Application.hpp"

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nShowCmd)
{
#if _DEBUG
	// Enable memory leak checker
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	int exitCode = EXIT_FAILURE;

	try
	{
		ApplicationPtr app = Application::Create(hInstance, nShowCmd);
		exitCode = app->MessagePump();
	}
	catch (const std::exception& e)
	{
		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");
		exitCode = EXIT_FAILURE;
	}

	return exitCode;
}