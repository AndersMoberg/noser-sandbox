// D3D11Driver.cpp
// Nolan Check
// Created 12/26/2011

#include "D3D11Driver.hpp"

#include "D3D11Utils.hpp"

namespace D3D11
{

D3D11DriverPtr D3D11Driver::Create()
{
	D3D11DriverPtr result(new D3D11Driver);
	if (!result->CreateInternal()) {
		return NULL;
	}
	return result;
}

D3D11Driver::D3D11Driver()
	: m_pD3D11Device(NULL),
	m_pD3D11Context(NULL)
{ }

D3D11Driver::~D3D11Driver()
{
	SafeRelease(m_pD3D11Context);
	SafeRelease(m_pD3D11Device);
}

bool D3D11Driver::CreateInternal()
{
	HRESULT hr;

	UINT createFlags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#if _DEBUG
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	hr = D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		createFlags,
		NULL, 0,
		D3D11_SDK_VERSION,
		&m_pD3D11Device,
		NULL,
		&m_pD3D11Context);
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

}
