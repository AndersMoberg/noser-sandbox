// WindowsUtils.hpp
// Nolan Check
// Created 2/27/2012

#ifndef _WINDOWSUTILS_HPP
#define _WINDOWSUTILS_HPP

#include <string>

#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

void Debug(const char* msg, ...);

#endif
