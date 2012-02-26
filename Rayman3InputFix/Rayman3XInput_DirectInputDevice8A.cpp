// Rayman3XInput_DirectInputDevice8A.cpp
// Nolan Check
// Created 2/25/2012

#include "Rayman3XInput_DirectInputDevice8A.hpp"

#include <Xinput.h>

HRESULT Rayman3XInput_DirectInputDevice8A::Create(Rayman3XInput_DirectInputDevice8A** ppvOut, int controller)
{
	HRESULT hr;

	*ppvOut = NULL;

	Rayman3XInput_DirectInputDevice8A* result = new Rayman3XInput_DirectInputDevice8A;
	if (result)
	{
		hr = result->CreateInternal(controller);
		if (FAILED(hr)) {
			result->Release();
		} else {
			*ppvOut = result;
		}
	}
	else
	{
		hr = E_OUTOFMEMORY;
	}

	return hr;
}

Rayman3XInput_DirectInputDevice8A::Rayman3XInput_DirectInputDevice8A()
	: m_refCount(1)
{ }

Rayman3XInput_DirectInputDevice8A::~Rayman3XInput_DirectInputDevice8A()
{ }

HRESULT Rayman3XInput_DirectInputDevice8A::CreateInternal(int controller)
{
	m_controller = controller;
	return DI_OK;
}

HRESULT Rayman3XInput_DirectInputDevice8A::QueryInterface(REFIID riid, void** ppvObject)
{
	IUnknown* pUnk = NULL;
	if (riid == IID_IUnknown) {
		pUnk = this;
	} else if (riid == IID_IDirectInputDevice8A) {
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

ULONG Rayman3XInput_DirectInputDevice8A::AddRef()
{
	return InterlockedIncrement(&m_refCount);
}

ULONG Rayman3XInput_DirectInputDevice8A::Release()
{
	ULONG c = InterlockedDecrement(&m_refCount);
	if (c == 0)
	{
		delete this;
	}
	return c;
}

HRESULT Rayman3XInput_DirectInputDevice8A::GetCapabilities(LPDIDEVCAPS lpDIDevCaps)
{
	OutputDebugStringA("R3XI: GetCapabilities called\n");

	if (lpDIDevCaps->dwSize != sizeof(DIDEVCAPS)) {
		return DIERR_NOTINITIALIZED;
	}

	lpDIDevCaps->dwFlags = DIDC_ATTACHED | DIDC_EMULATED | DIDC_POLLEDDATAFORMAT
		| DIDC_POLLEDDEVICE;
	lpDIDevCaps->dwDevType = DI8DEVTYPE_GAMEPAD | (DI8DEVTYPEGAMEPAD_STANDARD << 8);
	lpDIDevCaps->dwAxes = 4;
	lpDIDevCaps->dwButtons = 12; // The left and right triggers are treated as buttons
	lpDIDevCaps->dwPOVs = 1;
	lpDIDevCaps->dwFFSamplePeriod = 0;
	lpDIDevCaps->dwFFMinTimeResolution = 0;
	lpDIDevCaps->dwFirmwareRevision = 0;
	lpDIDevCaps->dwHardwareRevision = 0;
	lpDIDevCaps->dwFFDriverVersion = 0;

	return DI_OK;
}

HRESULT Rayman3XInput_DirectInputDevice8A::EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	OutputDebugStringA("R3XI: EnumObjects called\n");
	return E_NOTIMPL;
}

static void Debug(const char* msg, ...)
{
	va_list args = NULL;
	va_start(args, msg);

	int len = _vscprintf(msg, args) + 1;

	char* buf = new char[len];
	vsprintf_s(buf, len, msg, args);

	va_end(args);

	OutputDebugStringA(buf);
	delete[] buf;
}

HRESULT Rayman3XInput_DirectInputDevice8A::GetProperty(REFGUID rguidProp, LPDIPROPHEADER pdiph)
{
	OutputDebugStringA("R3XI: GetProperty called\n");
	if (&rguidProp == &DIPROP_RANGE)
	{
		if (pdiph->dwSize != sizeof(DIPROPRANGE) ||
			pdiph->dwHeaderSize != sizeof(DIPROPHEADER) ||
			pdiph->dwHow != DIPH_BYOFFSET)
		{
			Debug("Invalid DIPROP_RANGE pdiph\n");
			return DIERR_INVALIDPARAM;
		}

		Debug("Querying DIPROP_RANGE for object %d\n", pdiph->dwObj);

		LPDIPROPRANGE prop = (LPDIPROPRANGE)pdiph;
		prop->lMin = -32768;
		prop->lMax = 32767;
		return DI_OK;
	}
	else
	{
		Debug("Unsupported property %p\n", &rguidProp);
		return DIERR_UNSUPPORTED;
	}
}

HRESULT Rayman3XInput_DirectInputDevice8A::SetProperty(REFGUID rguidProp, LPCDIPROPHEADER pdiph)
{
	OutputDebugStringA("R3XI: SetProperty called\n");
	if (&rguidProp == &DIPROP_DEADZONE)
	{
		if (pdiph->dwSize != sizeof(DIPROPDWORD) ||
			pdiph->dwHeaderSize != sizeof(DIPROPHEADER) ||
			pdiph->dwObj != 0 ||
			pdiph->dwHow != DIPH_DEVICE)
		{
			Debug("Invalid DIPROP_DEADZONE pdiph\n");
			return DIERR_INVALIDPARAM;
		}

		LPDIPROPDWORD prop = (LPDIPROPDWORD)pdiph;
		Debug("DIPROP_DEADZONE set to %d\n", prop->dwData);
		return DI_OK;
	}
	else
	{
		Debug("Unsupported property %p\n", &rguidProp);
		return DIERR_UNSUPPORTED;
	}
}

HRESULT Rayman3XInput_DirectInputDevice8A::Acquire()
{
	OutputDebugStringA("R3XI: Acquire called\n");
	XInputGetState(m_controller, &m_controllerState);
	return DI_OK;
}

HRESULT Rayman3XInput_DirectInputDevice8A::Unacquire()
{
	OutputDebugStringA("R3XI: Unacquire called\n");
	return DI_OK;
}

HRESULT Rayman3XInput_DirectInputDevice8A::GetDeviceState(DWORD cbData, LPVOID lpvData)
{
	OutputDebugStringA("R3XI: GetDeviceState called\n");

	BYTE* data = (BYTE*)lpvData;
	for (DataFormatHandlers::const_iterator it = m_dataFormatHandlers.begin();
		it != m_dataFormatHandlers.end(); ++it)
	{
		it->func(&data[it->offset], m_controllerState);
	}

	return DI_OK;
}

HRESULT Rayman3XInput_DirectInputDevice8A::GetDeviceData(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD)
{
	OutputDebugStringA("R3XI: GetDeviceData called\n");
	return E_NOTIMPL;
}

HRESULT Rayman3XInput_DirectInputDevice8A::SetDataFormat(LPCDIDATAFORMAT lpdf)
{
	OutputDebugStringA("R3XI: SetDataFormat called\n");

	if (lpdf->dwSize != sizeof(DIDATAFORMAT) ||
		lpdf->dwObjSize != sizeof(DIOBJECTDATAFORMAT))
	{
		Debug("SetDataFormat called with invalid lpdf\n");
		return DIERR_INVALIDPARAM;
	}

	m_dataFormatHandlers.reserve(lpdf->dwNumObjs);

	for (DWORD i = 0; i < lpdf->dwNumObjs; ++i)
	{
		LPDIOBJECTDATAFORMAT odf = &lpdf->rgodf[i];

		Debug("Object %d:\n", i);

		if (odf->pguid == NULL) {
			Debug("  GUID: NULL\n");
		} else {
			OLECHAR* guidStr;
			StringFromCLSID(*odf->pguid, &guidStr);
			Debug("  GUID: %S\n", guidStr);
			CoTaskMemFree(guidStr);
		}

		Debug("  Offset: %d\n", odf->dwOfs);
		Debug("  Type: 0x%.08X\n", odf->dwType);
		Debug("  Flags: 0x%.08X\n", odf->dwFlags);

		if (odf->pguid == NULL)
		{
			if (DIDFT_GETTYPE(odf->dwType) == DIDFT_BUTTON)
			{
				m_dataFormatHandlers.push_back(
					DataFormatHandler(ZeroByteDataFormatHandler, odf->dwOfs));
			}
			else
			{
				Debug("Unknown data type %X in format\n", DIDFT_GETTYPE(odf->dwType));
				return DIERR_INVALIDPARAM;
			}
		}
		else if (*odf->pguid == GUID_XAxis)
		{
			m_dataFormatHandlers.push_back(
				DataFormatHandler(XAxisDataFormatHandler, odf->dwOfs));
		}
		else if (*odf->pguid == GUID_YAxis)
		{
			m_dataFormatHandlers.push_back(
				DataFormatHandler(YAxisDataFormatHandler, odf->dwOfs));
		}
		else if (*odf->pguid == GUID_ZAxis)
		{
			m_dataFormatHandlers.push_back(
				DataFormatHandler(ZeroDwordDataFormatHandler, odf->dwOfs));
		}
		else if (*odf->pguid == GUID_RxAxis)
		{
			m_dataFormatHandlers.push_back(
				DataFormatHandler(ZeroDwordDataFormatHandler, odf->dwOfs));
		}
		else if (*odf->pguid == GUID_RyAxis)
		{
			m_dataFormatHandlers.push_back(
				DataFormatHandler(ZeroDwordDataFormatHandler, odf->dwOfs));
		}
		else if (*odf->pguid == GUID_RzAxis)
		{
			m_dataFormatHandlers.push_back(
				DataFormatHandler(ZeroDwordDataFormatHandler, odf->dwOfs));
		}
		else if (*odf->pguid == GUID_Slider)
		{
			m_dataFormatHandlers.push_back(
				DataFormatHandler(ZeroDwordDataFormatHandler, odf->dwOfs));
		}
		else if (*odf->pguid == GUID_POV)
		{
			m_dataFormatHandlers.push_back(
				DataFormatHandler(ZeroDwordDataFormatHandler, odf->dwOfs));
		}
		else
		{
			Debug("Unknown object GUID\n");
			return DIERR_INVALIDPARAM;
		}
	}

	return DI_OK;
}

HRESULT Rayman3XInput_DirectInputDevice8A::SetEventNotification(THIS_ HANDLE)
{
	OutputDebugStringA("R3XI: SetEventNotification called\n");
	return E_NOTIMPL;
}

HRESULT Rayman3XInput_DirectInputDevice8A::SetCooperativeLevel(HWND hwnd, DWORD dwFlags)
{
	OutputDebugStringA("R3XI: SetCooperativeLevel called\n");
	return DI_OK;
}

HRESULT Rayman3XInput_DirectInputDevice8A::GetObjectInfo(THIS_ LPDIDEVICEOBJECTINSTANCEA,DWORD,DWORD)
{
	OutputDebugStringA("R3XI: GetObjectInfo called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::GetDeviceInfo(THIS_ LPDIDEVICEINSTANCEA)
{
	OutputDebugStringA("R3XI: GetDeviceInfo called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::RunControlPanel(THIS_ HWND,DWORD)
{
	OutputDebugStringA("R3XI: RunControlPanel called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::Initialize(THIS_ HINSTANCE,DWORD,REFGUID)
{
	OutputDebugStringA("R3XI: Initialize called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::CreateEffect(THIS_ REFGUID,LPCDIEFFECT,LPDIRECTINPUTEFFECT *,LPUNKNOWN)
{
	OutputDebugStringA("R3XI: CreateEffect called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::EnumEffects(THIS_ LPDIENUMEFFECTSCALLBACKA,LPVOID,DWORD)
{
	OutputDebugStringA("R3XI: EnumEffects called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::GetEffectInfo(THIS_ LPDIEFFECTINFOA,REFGUID)
{
	OutputDebugStringA("R3XI: GetEffectInfo called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::GetForceFeedbackState(THIS_ LPDWORD)
{
	OutputDebugStringA("R3XI: GetForceFeedbackState called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::SendForceFeedbackCommand(THIS_ DWORD)
{
	OutputDebugStringA("R3XI: SendForceFeedbackCommand called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::EnumCreatedEffectObjects(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK,LPVOID,DWORD)
{
	OutputDebugStringA("R3XI: EnumCreatedEffectObjects called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::Escape(THIS_ LPDIEFFESCAPE)
{
	OutputDebugStringA("R3XI: Escape called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::Poll(THIS)
{
	OutputDebugStringA("R3XI: Poll called\n");
	if (XInputGetState(m_controller, &m_controllerState) == ERROR_SUCCESS) {
		return DI_OK;
	} else {
		return DIERR_INPUTLOST;
	}
}
HRESULT Rayman3XInput_DirectInputDevice8A::SendDeviceData(THIS_ DWORD,LPCDIDEVICEOBJECTDATA,LPDWORD,DWORD)
{
	OutputDebugStringA("R3XI: SendDeviceData called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::EnumEffectsInFile(THIS_ LPCSTR,LPDIENUMEFFECTSINFILECALLBACK,LPVOID,DWORD)
{
	OutputDebugStringA("R3XI: EnumEffectsInFile called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::WriteEffectToFile(THIS_ LPCSTR,DWORD,LPDIFILEEFFECT,DWORD)
{
	OutputDebugStringA("R3XI: WriteEffectToFile called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::BuildActionMap(THIS_ LPDIACTIONFORMATA,LPCSTR,DWORD)
{
	OutputDebugStringA("R3XI: BuildActionMap called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::SetActionMap(THIS_ LPDIACTIONFORMATA,LPCSTR,DWORD)
{
	OutputDebugStringA("R3XI: SetActionMap called\n");
	return E_NOTIMPL;
}
HRESULT Rayman3XInput_DirectInputDevice8A::GetImageInfo(THIS_ LPDIDEVICEIMAGEINFOHEADERA)
{
	OutputDebugStringA("R3XI: GetImageInfo called\n");
	return E_NOTIMPL;
}
