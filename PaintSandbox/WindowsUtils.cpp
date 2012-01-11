// WindowsUtils.cpp
// Nolan Check
// Created 1/10/2012

#include "WindowsUtils.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <cstdarg>
#include <cstdio>

void Debug(const char* msg, ...)
{
	va_list args = NULL;
	va_start(args, msg);

	int len = _vscprintf(msg, args) + 1;

	char* buf = new char[len];
	vsprintf_s(buf, len, msg, args);

	va_end(args);

	OutputDebugStringA(buf);
	delete[] buf;
}
