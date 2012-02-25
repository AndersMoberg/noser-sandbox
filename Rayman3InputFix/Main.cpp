// Main.cpp
// Nolan Check
// Created 2/25/2012

#include <InitGuid.h>
#include <dinput.h>

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
		return m_realDirectInput->EnumDevices(dwDevType, lpCallback, pvRef, dwFlags);
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
	// TODO: Use Windows installation directory instead of hardcoding it
	m_realDInput8Dll = LoadLibrary(TEXT("C:\\WINDOWS\\System32\\dinput8.dll"));
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
