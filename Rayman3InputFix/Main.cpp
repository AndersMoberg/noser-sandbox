// Main.cpp
// Nolan Check
// Created 2/25/2012

#include <InitGuid.h>
#include <dinput.h>
#include <Xinput.h>
#include <wbemidl.h>
#include <oleauto.h>

#include <list>
#include <string>

#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

// {DE1A8EA2-6FEB-4F4D-8792-2B095A68530F}
static const GUID GUID_RAYMAN3_XINPUT_PRODUCT = 
{ 0xde1a8ea2, 0x6feb, 0x4f4d, { 0x87, 0x92, 0x2b, 0x9, 0x5a, 0x68, 0x53, 0xf } };
// {EA386490-A172-44AA-9E0A-49AA43A83348}
static const GUID GUID_RAYMAN3_XINPUT_INSTANCE_1 = 
{ 0xea386490, 0xa172, 0x44aa, { 0x9e, 0xa, 0x49, 0xaa, 0x43, 0xa8, 0x33, 0x48 } };
// {42F5F7F5-0E76-4009-8DCE-B5797E1FE371}
static const GUID GUID_RAYMAN3_XINPUT_INSTANCE_2 = 
{ 0x42f5f7f5, 0xe76, 0x4009, { 0x8d, 0xce, 0xb5, 0x79, 0x7e, 0x1f, 0xe3, 0x71 } };
// {0070CFAB-2B48-4958-AD34-709EC94F0997}
static const GUID GUID_RAYMAN3_XINPUT_INSTANCE_3 = 
{ 0x70cfab, 0x2b48, 0x4958, { 0xad, 0x34, 0x70, 0x9e, 0xc9, 0x4f, 0x9, 0x97 } };
// {69FA7368-92A4-4C12-9FC2-B0B59F73933F}
static const GUID GUID_RAYMAN3_XINPUT_INSTANCE_4 = 
{ 0x69fa7368, 0x92a4, 0x4c12, { 0x9f, 0xc2, 0xb0, 0xb5, 0x9f, 0x73, 0x93, 0x3f } };

static const char RAYMAN3_XINPUT_PRODUCT_NAME[] = "Rayman 3 compatible XInput device";

static const char RAYMAN3_XINPUT_INSTANCE_NAME_1[] = "Rayman 3 compatible XInput device 1";
static const char RAYMAN3_XINPUT_INSTANCE_NAME_2[] = "Rayman 3 compatible XInput device 2";
static const char RAYMAN3_XINPUT_INSTANCE_NAME_3[] = "Rayman 3 compatible XInput device 3";
static const char RAYMAN3_XINPUT_INSTANCE_NAME_4[] = "Rayman 3 compatible XInput device 4";

static const char* RAYMAN3_XINPUT_INSTANCE_NAMES[4] = {
	RAYMAN3_XINPUT_INSTANCE_NAME_1,
	RAYMAN3_XINPUT_INSTANCE_NAME_2,
	RAYMAN3_XINPUT_INSTANCE_NAME_3,
	RAYMAN3_XINPUT_INSTANCE_NAME_4
};

static const GUID* RAYMAN3_XINPUT_INSTANCE_GUIDS[4] = {
	&GUID_RAYMAN3_XINPUT_INSTANCE_1,
	&GUID_RAYMAN3_XINPUT_INSTANCE_2,
	&GUID_RAYMAN3_XINPUT_INSTANCE_3,
	&GUID_RAYMAN3_XINPUT_INSTANCE_4
};


template<class Interface>
inline void SafeRelease(Interface*& pInterface)
{
	if (pInterface)
	{
		pInterface->Release();
		pInterface = NULL;
	}
}

// This function comes from the article "XInput and DirectInput" in the DirectX SDK
//-----------------------------------------------------------------------------
// Enum each PNP device using WMI and check each device ID to see if it contains 
// "IG_" (ex. "VID_045E&PID_028E&IG_00").  If it does, then it's an XInput device
// Unfortunately this information can not be found by just using DirectInput 
//-----------------------------------------------------------------------------
BOOL IsXInputDevice( const GUID* pGuidProductFromDirectInput )
{
    IWbemLocator*           pIWbemLocator  = NULL;
    IEnumWbemClassObject*   pEnumDevices   = NULL;
    IWbemClassObject*       pDevices[20]   = {0};
    IWbemServices*          pIWbemServices = NULL;
    BSTR                    bstrNamespace  = NULL;
    BSTR                    bstrDeviceID   = NULL;
    BSTR                    bstrClassName  = NULL;
    DWORD                   uReturned      = 0;
    bool                    bIsXinputDevice= false;
    UINT                    iDevice        = 0;
    VARIANT                 var;
    HRESULT                 hr;

    // CoInit if needed
    hr = CoInitialize(NULL);
    bool bCleanupCOM = SUCCEEDED(hr);

    // Create WMI
    hr = CoCreateInstance( __uuidof(WbemLocator),
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           __uuidof(IWbemLocator),
                           (LPVOID*) &pIWbemLocator);
    if( FAILED(hr) || pIWbemLocator == NULL )
        goto LCleanup;

    bstrNamespace = SysAllocString( L"\\\\.\\root\\cimv2" );if( bstrNamespace == NULL ) goto LCleanup;        
    bstrClassName = SysAllocString( L"Win32_PNPEntity" );   if( bstrClassName == NULL ) goto LCleanup;        
    bstrDeviceID  = SysAllocString( L"DeviceID" );          if( bstrDeviceID == NULL )  goto LCleanup;        
    
    // Connect to WMI 
    hr = pIWbemLocator->ConnectServer( bstrNamespace, NULL, NULL, 0L, 
                                       0L, NULL, NULL, &pIWbemServices );
    if( FAILED(hr) || pIWbemServices == NULL )
        goto LCleanup;

    // Switch security level to IMPERSONATE. 
    CoSetProxyBlanket( pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, 
                       RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE );                    

    hr = pIWbemServices->CreateInstanceEnum( bstrClassName, 0, NULL, &pEnumDevices ); 
    if( FAILED(hr) || pEnumDevices == NULL )
        goto LCleanup;

    // Loop over all devices
    for( ;; )
    {
        // Get 20 at a time
        hr = pEnumDevices->Next( 10000, 20, pDevices, &uReturned );
        if( FAILED(hr) )
            goto LCleanup;
        if( uReturned == 0 )
            break;

        for( iDevice=0; iDevice<uReturned; iDevice++ )
        {
            // For each device, get its device ID
            hr = pDevices[iDevice]->Get( bstrDeviceID, 0L, &var, NULL, NULL );
            if( SUCCEEDED( hr ) && var.vt == VT_BSTR && var.bstrVal != NULL )
            {
                // Check if the device ID contains "IG_".  If it does, then it's an XInput device
				    // This information can not be found from DirectInput 
                if( wcsstr( var.bstrVal, L"IG_" ) )
                {
                    // If it does, then get the VID/PID from var.bstrVal
                    DWORD dwPid = 0, dwVid = 0;
                    WCHAR* strVid = wcsstr( var.bstrVal, L"VID_" );
                    if( strVid && swscanf_s( strVid, L"VID_%4X", &dwVid ) != 1 )
                        dwVid = 0;
                    WCHAR* strPid = wcsstr( var.bstrVal, L"PID_" );
                    if( strPid && swscanf_s( strPid, L"PID_%4X", &dwPid ) != 1 )
                        dwPid = 0;

                    // Compare the VID/PID to the DInput device
                    DWORD dwVidPid = MAKELONG( dwVid, dwPid );
                    if( dwVidPid == pGuidProductFromDirectInput->Data1 )
                    {
                        bIsXinputDevice = true;
                        goto LCleanup;
                    }
                }
            }   
            SafeRelease( pDevices[iDevice] );
        }
    }

LCleanup:
    if(bstrNamespace)
        SysFreeString(bstrNamespace);
    if(bstrDeviceID)
        SysFreeString(bstrDeviceID);
    if(bstrClassName)
        SysFreeString(bstrClassName);
    for( iDevice=0; iDevice<20; iDevice++ )
        SafeRelease( pDevices[iDevice] );
    SafeRelease( pEnumDevices );
    SafeRelease( pIWbemLocator );
    SafeRelease( pIWbemServices );

    if( bCleanupCOM )
        CoUninitialize();

    return bIsXinputDevice;
}

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

		// See if one of the XInput devices was requested
		int xinputDevice = -1;
		for (int i = 0; i < 4; ++i)
		{
			if (rguid == *RAYMAN3_XINPUT_INSTANCE_GUIDS[i])
			{
				xinputDevice = i;
				break;
			}
		}

		if (xinputDevice != -1)
		{
			OutputDebugStringA("XInput device created\n");
			return DIERR_DEVICENOTREG;
		}
		else
		{
			return m_realDirectInput->CreateDevice(rguid, lplpDirectInputDevice, pUnkOuter);
		}
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
				// Filter out XInput devices
				if (!IsXInputDevice(&lpddi->guidProduct)) {
					self->devices.push_back(*lpddi);
				} else {
					OutputDebugStringA("XInput device filtered\n");
				}
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

		// First, add XInput gamepads
		for (int i = 0; i < 4; ++i)
		{
			XINPUT_CAPABILITIES caps;
			bool attached = XInputGetCapabilities(i, XINPUT_FLAG_GAMEPAD, &caps) == ERROR_SUCCESS;
			if (attached)
			{
				DIDEVICEINSTANCEA didi = { 0 };
				didi.dwSize = sizeof(DIDEVICEINSTANCEA);
				didi.guidInstance = *RAYMAN3_XINPUT_INSTANCE_GUIDS[i];
				didi.guidProduct = GUID_RAYMAN3_XINPUT_PRODUCT;
				didi.dwDevType = DI8DEVTYPE_GAMEPAD | (DI8DEVTYPEGAMEPAD_STANDARD << 8);
				strcpy_s(didi.tszInstanceName, RAYMAN3_XINPUT_INSTANCE_NAMES[i]);
				strcpy_s(didi.tszProductName, RAYMAN3_XINPUT_PRODUCT_NAME);
				sortedDevices.push_back(didi);
			}
		}

		// Add all devices in gameDevices
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
