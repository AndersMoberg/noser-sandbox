// Main.cpp
// Nolan Check
// Created 3/18/2012

#include "Rayman2InputFix_DirectInputA.hpp"

#include "WindowsUtils.hpp"

HRESULT WINAPI DirectInputCreateA(HINSTANCE hinst, DWORD dwVersion,
	LPDIRECTINPUTA* ppDI, LPUNKNOWN punkOuter)
{
	Debug("DirectInputCreateA called\n");
	Debug("dwVersion == 0x%X\n", dwVersion);

	HRESULT hr;

	*ppDI = NULL;

	Rayman2InputFix_DirectInputA* result = NULL;
	hr = Rayman2InputFix_DirectInputA::Create(hinst, &result);
	if (SUCCEEDED(hr))
	{
		Debug("Rayman2InputFix DirectInput interface created. Initializing...\n");
		hr = result->Initialize(hinst, dwVersion);
		Debug("(Initialize called)\n");
		if (SUCCEEDED(hr)) {
			Debug("Initialized successfully\n");
			*ppDI = result;
		}
	}

	return hr;
}
