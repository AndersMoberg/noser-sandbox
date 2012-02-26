// Rayman3XInput_DirectInputDevice8A.hpp
// Nolan Check
// Created 2/25/2012

#ifndef _RAYMAN3XINPUT_DIRECTINPUTDEVICE8A_HPP
#define _RAYMAN3XINPUT_DIRECTINPUTDEVICE8A_HPP

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

class Rayman3XInput_DirectInputDevice8A : public IDirectInputDevice8A
{

public:

	static HRESULT Create(Rayman3XInput_DirectInputDevice8A** ppvOut);
	
	// IUnknown
	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// IDirectInputDevice8A
	STDMETHOD(GetCapabilities)(LPDIDEVCAPS lpDIDevCaps);
	STDMETHOD(EnumObjects)(LPDIENUMDEVICEOBJECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags);
	STDMETHOD(GetProperty)(REFGUID rguidProp, LPDIPROPHEADER pdiph);
	STDMETHOD(SetProperty)(REFGUID rguidProp, LPCDIPROPHEADER pdiph);
	STDMETHOD(Acquire)();
	STDMETHOD(Unacquire)();
	STDMETHOD(GetDeviceState)(DWORD cbData, LPVOID lpvData);

private:

	Rayman3XInput_DirectInputDevice8A();
	~Rayman3XInput_DirectInputDevice8A();

	HRESULT CreateInternal();

	ULONG m_refCount;

};

#endif
