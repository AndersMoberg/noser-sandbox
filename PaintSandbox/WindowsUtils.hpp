// WindowsUtils.hpp
// Nolan Check
// Created 1/10/2012

#ifndef _WINDOWSUTILS_HPP
#define _WINDOWSUTILS_HPP

#define CHECK_HR(hr) \
	do { \
		if (FAILED(hr)) { \
			throw std::exception("Failed: " #hr); \
		} \
	} while (0)

void Debug(const char* msg, ...);

#endif
