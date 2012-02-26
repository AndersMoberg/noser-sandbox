// Main.cpp
// Nolan Check
// Created 2/25/2012

#include <InitGuid.h>
#include <dinput.h>

#include <list>
#include <string>

#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

class Rayman3InputFix_DirectInput8A : public IDirectInput8A
{

public:

	static HRESULT Create(REFIID riid, void** ppvObject, HINSTANCE hInstance);

	// IUnknown implementation

	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject)
	{
		IUnknown* pUnk = NULL;
		if (riid == IID_IUnknown) {
			pUnk = this;
		} else if (riid == IID_IDirectInput8A) {
			pUnk = this;
		}

		*ppvObject = pUnk;
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

	STDMETHOD_(ULONG, AddRef)()
	{
		return InterlockedIncrement(&m_refCount);
	}

	STDMETHOD_(ULONG, Release)()
	{
		ULONG c = InterlockedDecrement(&m_refCount);
		if (c == 0)
		{
			delete this;
		}
		return c;
	}

	// IDirectInput8A implementation

	STDMETHOD(CreateDevice)(REFGUID rguid, LPDIRECTINPUTDEVICE8A* lplpDirectInputDevice, LPUNKNOWN pUnkOuter)
	{
		OutputDebugStringA("CreateDevice called\n");
		return m_realDirectInput->CreateDevice(rguid, lplpDirectInputDevice, pUnkOuter);
	}

	STDMETHOD(EnumDevices)(DWORD dwDevType, LPDIENUMDEVICESCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags)
	{
		OutputDebugStringA("EnumDevices called\n");
		
		HRESULT hr;

		// The bug: Rayman 3 expects EnumDevices to give results in a certain
		// order, where gamepads come before the keyboard. DirectInput makes
		// no guarantee about the order.
		// The fix: Call DirectInput's EnumDevices, then sort the results in
		// an order where gamepads come first, then give them to Rayman 3.

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

		DeviceEnumerator gameDevices;
		hr = m_realDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL, DeviceEnumerator::Callback, &gameDevices, dwFlags);
		if (FAILED(hr)) {
			return hr;
		}

		DeviceEnumerator allDevices;
		hr = m_realDirectInput->EnumDevices(DI8DEVCLASS_ALL, DeviceEnumerator::Callback, &allDevices, dwFlags);
		if (FAILED(hr)) {
			return hr;
		}

		DeviceInstanceList sortedDevices;
		// First, add all devices in gameDevices
		for (DeviceInstanceList::const_iterator it = gameDevices.devices.begin();
			it != gameDevices.devices.end(); ++it)
		{
			sortedDevices.push_back(*it);
		}

		// Then, add all devices in allDevices that aren't in gameDevices
		for (DeviceInstanceList::const_iterator it = allDevices.devices.begin();
			it != allDevices.devices.end(); ++it)
		{
			if (!gameDevices.Contains(it->guidInstance)) {
				sortedDevices.push_back(*it);
			}
		}

		if (dwDevType == DI8DEVCLASS_ALL)
		{
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
		}
		else
		{
			// Rayman 3 never does this
			OutputDebugStringA("EnumDevices called with dwDevType != DI8DEVCLASS_ALL!\n");
			return DIERR_INVALIDPARAM;
		}
	}

	STDMETHOD(GetDeviceStatus)(REFGUID rguidInstance)
	{
		OutputDebugStringA("GetDeviceStatus called\n");
		return E_NOTIMPL;
	}

	STDMETHOD(RunControlPanel)(HWND hwndOwner, DWORD dwFlags)
	{
		OutputDebugStringA("RunControlPanel called\n");
		return E_NOTIMPL;
	}

	STDMETHOD(Initialize)(HINSTANCE hinst, DWORD dwVersion)
	{
		OutputDebugStringA("Initialize called\n");
		if (dwVersion == 0x0800) {
			return DI_OK;
		} else if (dwVersion > 0x0800) {
			return DIERR_OLDDIRECTINPUTVERSION;
		} else { // dwVersion < 0x0800
			return DIERR_BETADIRECTINPUTVERSION;
		}
	}

	STDMETHOD(FindDevice)(REFGUID rguidClass, LPCSTR ptszName, LPGUID pguidInstance)
	{
		OutputDebugStringA("FindDevice called\n");
		return E_NOTIMPL;
	}

	STDMETHOD(EnumDevicesBySemantics)(LPCSTR ptszUserName, LPDIACTIONFORMATA lpdiActionFormat, LPDIENUMDEVICESBYSEMANTICSCBA lpCallback, LPVOID pvRef, DWORD dwFlags)
	{
		OutputDebugStringA("EnumDevicesBySemantics called\n");
		return E_NOTIMPL;
	}

	STDMETHOD(ConfigureDevices)(LPDICONFIGUREDEVICESCALLBACK lpdiCallback, LPDICONFIGUREDEVICESPARAMSA lpdiCDParams, DWORD dwFlags, LPVOID pvRefData)
	{
		OutputDebugStringA("ConfigureDevices called\n");
		return E_NOTIMPL;
	}

private:

	Rayman3InputFix_DirectInput8A();
	~Rayman3InputFix_DirectInput8A();

	HRESULT CreateInternal(HINSTANCE hInstance);

	ULONG m_refCount;
	HMODULE m_realDInput8Dll;
	IDirectInput8A* m_realDirectInput;

};

HRESULT Rayman3InputFix_DirectInput8A::Create(REFIID riid, void** ppvObject, HINSTANCE hInstance)
{
	HRESULT hr;

	if (ppvObject == NULL)
	{
		hr = E_POINTER;
	}
	else
	{
		*ppvObject = NULL;

		Rayman3InputFix_DirectInput8A* result = new Rayman3InputFix_DirectInput8A;
		if (result)
		{
			hr = result->CreateInternal(hInstance);
			if (SUCCEEDED(hr))
			{
				hr = result->QueryInterface(riid, ppvObject);
				result->Release();
			}
		}
		else
		{
			hr = E_OUTOFMEMORY;
		}
	}

	return hr;
}

Rayman3InputFix_DirectInput8A::Rayman3InputFix_DirectInput8A()
	: m_refCount(1),
	m_realDInput8Dll(NULL),
	m_realDirectInput(NULL)
{ }

Rayman3InputFix_DirectInput8A::~Rayman3InputFix_DirectInput8A()
{
	if (m_realDirectInput)
	{
		m_realDirectInput->Release();
		m_realDirectInput = NULL;
	}

	if (m_realDInput8Dll)
	{
		FreeLibrary(m_realDInput8Dll);
		m_realDInput8Dll = NULL;
	}
}

typedef HRESULT (WINAPI *PDIRECTINPUT8CREATE)(HINSTANCE hinst, DWORD dwVersion,
	REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter);

HRESULT Rayman3InputFix_DirectInput8A::CreateInternal(HINSTANCE hInstance)
{
	HRESULT hr;

	// Load the system's dinput8.dll

	UINT len = GetSystemDirectory(NULL, 0);
	TCHAR* systemDir = new TCHAR[len];
	if (!systemDir) {
		return E_OUTOFMEMORY;
	}
	GetSystemDirectory(systemDir, len);
	tstring dinput8Path(systemDir);
	delete[] systemDir;

	dinput8Path.append(TEXT("\\dinput8.dll"));
	m_realDInput8Dll = LoadLibrary(dinput8Path.c_str());
	if (!m_realDInput8Dll) {
		return E_FAIL;
	}

	PDIRECTINPUT8CREATE p_DirectInput8Create = (PDIRECTINPUT8CREATE)GetProcAddress(m_realDInput8Dll, "DirectInput8Create");
	if (!p_DirectInput8Create) {
		return E_FAIL;
	}

	hr = p_DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8A, (void**)&m_realDirectInput, NULL);

	return hr;
}

HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion,
	REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
{
	HRESULT hr;

	if (riidltf == IID_IDirectInput8A)
	{
		hr = Rayman3InputFix_DirectInput8A::Create(riidltf, ppvOut, hinst);
		if (SUCCEEDED(hr))
		{
			IDirectInput8A* p = (IDirectInput8A*)*ppvOut;
			hr = p->Initialize(hinst, dwVersion);
		}
	}
	else
	{
		hr = DIERR_INVALIDPARAM;
	}

	return hr;
}
