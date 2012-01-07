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
	ApplicationPtr app = Application::Create(hInstance, nShowCmd);
	if (!app) {
		return EXIT_FAILURE;
	}

	return app->MessageLoop();
}
