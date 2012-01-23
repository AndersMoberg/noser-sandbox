// WindowsUtils.hpp
// Nolan Check
// Created 1/23/2012

#ifndef _WINDOWSUTILS_HPP
#define _WINDOWSUTILS_HPP

template<class Interface>
inline void SafeRelease(Interface*& pInterface)
{
	if (pInterface)
	{
		pInterface->Release();
		pInterface = NULL;
	}
}

#define CHECK_HR(x) \
	do { \
		if (FAILED(x)) { \
			throw std::exception("Failed: " #x); \
		} \
	} while (0)

#endif
