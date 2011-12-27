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
	m_pD3D11Context(NULL),
	m_pDXGIFactory(NULL)
{ }

D3D11Driver::~D3D11Driver()
{
	SafeRelease(m_pDXGIFactory);
	SafeRelease(m_pD3D11Context);
	SafeRelease(m_pD3D11Device);
}

bool D3D11Driver::CreateInternal()
{
	HRESULT hr;

	// Create device

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

	// Extract DXGI factory from device

	IDXGIDevice* dxgiDevice = NULL;
	hr = m_pD3D11Device->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
	if (FAILED(hr)) {
		return false;
	}

	IDXGIAdapter* dxgiAdapter = NULL;
	hr = dxgiDevice->GetAdapter(&dxgiAdapter);
	if (FAILED(hr)) {
		SafeRelease(dxgiDevice);
		return false;
	}

	SafeRelease(dxgiDevice);

	hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&m_pDXGIFactory));
	if (FAILED(hr)) {
		SafeRelease(dxgiAdapter);
		return false;
	}

	SafeRelease(dxgiAdapter);

	return true;
}

CanvasWindowGraphicsPtr D3D11Driver::CreateWindowGraphics(HWND hWnd)
{
	return D3D11CanvasWindowGraphics::Create(hWnd, m_pD3D11Device, m_pDXGIFactory);
}

}
