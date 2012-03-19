// Rayman2InputFix_DirectInputA.hpp
// Nolan Check
// Created 3/18/2012

#ifndef _RAYMAN2INPUTFIX_DIRECTINPUTA_HPP
#define _RAYMAN2INPUTFIX_DIRECTINPUTA_HPP

#include <InitGuid.h>
#define DIRECTINPUT_VERSION 0x0700
#include <dinput.h>

class Rayman2InputFix_DirectInputA : public IDirectInputA
{

public:

	static HRESULT Create(HINSTANCE hinst, Rayman2InputFix_DirectInputA** ppDI);
	
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);
    STDMETHOD_(ULONG,AddRef)(THIS);
    STDMETHOD_(ULONG,Release)(THIS);

    /*** IDirectInputA methods ***/
    STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICEA *,LPUNKNOWN);
    STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACKA,LPVOID,DWORD);
    STDMETHOD(GetDeviceStatus)(THIS_ REFGUID);
    STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD);
    STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD);

private:

	Rayman2InputFix_DirectInputA();
	~Rayman2InputFix_DirectInputA();

	HRESULT CreateInternal(HINSTANCE hinst);

	ULONG m_refCount;
	HMODULE m_realDInputDll;
	IDirectInputA* m_realDirectInput;

};

#endif
