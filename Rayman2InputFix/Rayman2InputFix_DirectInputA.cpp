// Rayman2InputFix_DirectInputA.cpp
// Nolan Check
// Created 3/18/2012

#include "Rayman2InputFix_DirectInputA.hpp"

#include <list>

#include "WindowsUtils.hpp"

Rayman2InputFix_DirectInputA::Rayman2InputFix_DirectInputA()
	: m_refCount(1),
	m_realDInputDll(NULL),
	m_realDirectInput(NULL)
{ }

Rayman2InputFix_DirectInputA::~Rayman2InputFix_DirectInputA()
{
	if (m_realDirectInput)
	{
		m_realDirectInput->Release();
		m_realDirectInput = NULL;
	}

	if (m_realDInputDll)
	{
		FreeLibrary(m_realDInputDll);
		m_realDInputDll = NULL;
	}
}

HRESULT Rayman2InputFix_DirectInputA::Create(HINSTANCE hinst, Rayman2InputFix_DirectInputA** ppDI)
{
	HRESULT hr;

	if (ppDI == NULL)
	{
		hr = E_POINTER;
	}
	else
	{
		*ppDI = NULL;

		Rayman2InputFix_DirectInputA* result = new Rayman2InputFix_DirectInputA;
		if (result)
		{
			hr = result->CreateInternal(hinst);
			if (SUCCEEDED(hr)) {
				*ppDI = result;
			}
		}
		else
		{
			hr = E_OUTOFMEMORY;
		}
	}

	return hr;
}

typedef HRESULT (WINAPI *PDIRECTINPUTCREATEA)(HINSTANCE hinst, DWORD dwVersion,
	LPDIRECTINPUTA *ppDI, LPUNKNOWN punkOuter);

HRESULT Rayman2InputFix_DirectInputA::CreateInternal(HINSTANCE hinst)
{
	HRESULT hr;

	// Load the system's dinput8.dll

	UINT len = GetSystemDirectory(NULL, 0);
	TCHAR* systemDir = new TCHAR[len];
	if (!systemDir) {
		return E_OUTOFMEMORY;
	}
	GetSystemDirectory(systemDir, len);
	tstring dinputPath(systemDir);
	delete[] systemDir;

	dinputPath.append(TEXT("\\dinput.dll"));
	m_realDInputDll = LoadLibrary(dinputPath.c_str());
	if (!m_realDInputDll) {
		return E_FAIL;
	}

	PDIRECTINPUTCREATEA p_DirectInputCreateA = (PDIRECTINPUTCREATEA)GetProcAddress(m_realDInputDll, "DirectInputCreateA");
	if (!p_DirectInputCreateA) {
		return E_FAIL;
	}

	Debug("Calling real DirectInputCreateA...\n");
	hr = p_DirectInputCreateA(hinst, DIRECTINPUT_VERSION, &m_realDirectInput, NULL);
	Debug("Real DirectInput interface created\n");

	return hr;
}

/*** IUnknown methods ***/
HRESULT Rayman2InputFix_DirectInputA::QueryInterface(REFIID riid, LPVOID * ppvObj)
{
	Debug("DI: QueryInterface called\n");
	IUnknown* pUnk = NULL;
	if (riid == IID_IUnknown) {
		pUnk = this;
	} else if (riid == IID_IDirectInputA) {
		pUnk = this;
	}

	*ppvObj = pUnk;
	if (pUnk)
	{
		pUnk->AddRef();
		return S_OK;
	}
	else
	{
		return E_NOINTERFACE;
	}
}

ULONG Rayman2InputFix_DirectInputA::AddRef()
{
	Debug("DI: AddRef called\n");
	return InterlockedIncrement(&m_refCount);
}

ULONG Rayman2InputFix_DirectInputA::Release()
{
	Debug("DI: Release called\n");
	ULONG c = InterlockedDecrement(&m_refCount);
	if (c == 0) {
		delete this;
	}
	return c;
}

/*** IDirectInputA methods ***/
HRESULT Rayman2InputFix_DirectInputA::CreateDevice(REFGUID rguid,
	LPDIRECTINPUTDEVICEA* lplpDirectInputDevice, LPUNKNOWN pUnkOuter)
{
	Debug("DI: CreateDevice called\n");
	return m_realDirectInput->CreateDevice(rguid, lplpDirectInputDevice, pUnkOuter);
}

HRESULT Rayman2InputFix_DirectInputA::EnumDevices(DWORD dwDevType,
	LPDIENUMDEVICESCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	Debug("DI: EnumDevices called\n");
	Debug("dwDevType == %d\n", dwDevType);
#if 1
	HRESULT hr;

	// The bug: Rayman 2 expects EnumDevices to give results in a certain
	// order, where gamepads come before the keyboard. DirectInput makes
	// no guarantee about the order.
	// The fix: Call DirectInput's EnumDevices, then sort the results in
	// an order where gamepads come first, then give them to Rayman 2.

	typedef std::list<DIDEVICEINSTANCEA> DeviceInstanceList;
	struct DeviceEnumerator
	{
		DeviceInstanceList devices;

		static BOOL FAR PASCAL Callback(LPCDIDEVICEINSTANCEA lpddi, LPVOID pvRef)
		{
			DeviceEnumerator* self = (DeviceEnumerator*)pvRef;
			self->devices.push_back(*lpddi);
			return DIENUM_CONTINUE;
		}

		bool Contains(const GUID& guidInstance)
		{
			for (DeviceInstanceList::const_iterator it = devices.begin();
				it != devices.end(); ++it)
			{
				if (it->guidInstance == guidInstance) {
					return true;
				}
			}
			return false;
		}
	};

	DeviceEnumerator joystickDevices;
	hr = m_realDirectInput->EnumDevices(DIDEVTYPE_JOYSTICK, DeviceEnumerator::Callback, &joystickDevices, dwFlags);
	if (FAILED(hr)) {
		return hr;
	}

	DeviceEnumerator allDevices;
	hr = m_realDirectInput->EnumDevices(0, DeviceEnumerator::Callback, &allDevices, dwFlags);
	if (FAILED(hr)) {
		return hr;
	}

	DeviceInstanceList sortedDevices;

	if (dwDevType == 0 || dwDevType == DIDEVTYPE_JOYSTICK)
	{
		// Add all devices in gameDevices
		for (DeviceInstanceList::const_iterator it = joystickDevices.devices.begin();
			it != joystickDevices.devices.end(); ++it)
		{
			sortedDevices.push_back(*it);
		}
	}

	if (dwDevType == 0)
	{
		// Then, add all devices in allDevices that aren't in gameDevices
		for (DeviceInstanceList::const_iterator it = allDevices.devices.begin();
			it != allDevices.devices.end(); ++it)
		{
			if (!joystickDevices.Contains(it->guidInstance)) {
				sortedDevices.push_back(*it);
			}
		}
	}

	for (DeviceInstanceList::const_iterator it = sortedDevices.begin();
		it != sortedDevices.end(); ++it)
	{
		OutputDebugStringA("Enumerating Product: ");
		OutputDebugStringA(it->tszProductName);
		OutputDebugStringA(" Instance: ");
		OutputDebugStringA(it->tszInstanceName);
		OutputDebugStringA("\n");
		if (lpCallback(&*it, pvRef) == DIENUM_STOP) {
			break;
		}
	}

	return DI_OK;
#else
	return m_realDirectInput->EnumDevices(dwDevType, lpCallback, pvRef, dwFlags);
#endif
}

HRESULT Rayman2InputFix_DirectInputA::GetDeviceStatus(REFGUID rguidInstance)
{
	Debug("DI: GetDeviceStatus called\n");
	return m_realDirectInput->GetDeviceStatus(rguidInstance);
}

HRESULT Rayman2InputFix_DirectInputA::RunControlPanel(HWND hwndOwner, DWORD dwFlags)
{
	Debug("DI: RunControlPanel called\n");
	return m_realDirectInput->RunControlPanel(hwndOwner, dwFlags);
}

HRESULT Rayman2InputFix_DirectInputA::Initialize(HINSTANCE hinst, DWORD dwVersion)
{
	Debug("DI: Initialize called\n");
	return S_OK;
}
