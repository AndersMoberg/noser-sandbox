// D2DTarget.cpp
// Nolan Check
// Created 1/10/2012

#include "D2DTarget.hpp"

#include "D3D11Utils.hpp"

namespace D3D11
{

static const DXGI_FORMAT D2DTEXTURE_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

D2DTarget::D2DTarget()
	: m_pD2DTexture(NULL),
	m_pD2DTextureSRV(NULL),
	m_pD3D11Mutex(NULL),
	m_pD2DMutex(NULL),
	m_pD2DTarget(NULL)
{ }

D2DTarget::~D2DTarget()
{
	SafeRelease(m_pD2DTarget);
	SafeRelease(m_pD2DMutex);
	SafeRelease(m_pD3D11Mutex);
	SafeRelease(m_pD2DTextureSRV);
	SafeRelease(m_pD2DTexture);
}

D2DTargetPtr D2DTarget::Create(ID2D1Factory* pD2DFactory,
	ID3D11Device* pD3D11Device, ID3D10Device1* pD3D10Device,
	UINT width, UINT height)
{
	D2DTargetPtr p(new D2DTarget);

	HRESULT hr;

	// Create texture for rendering into

	D3D11_TEXTURE2D_DESC t2dd = CD3D11_TEXTURE2D_DESC(D2DTEXTURE_FORMAT,
		width, height, 1, 1, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		D3D11_USAGE_DEFAULT, 0, 1, 0, D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX);
	hr = pD3D11Device->CreateTexture2D(&t2dd, NULL, &p->m_pD2DTexture);
	if (FAILED(hr)) {
		return NULL;
	}

	// Create SRV for texture

	hr = pD3D11Device->CreateShaderResourceView(p->m_pD2DTexture, NULL, &p->m_pD2DTextureSRV);
	if (FAILED(hr)) {
		return NULL;
	}

	// Obtain mutex for D3D11

	hr = p->m_pD2DTexture->QueryInterface(IID_PPV_ARGS(&p->m_pD3D11Mutex));
	if (FAILED(hr)) {
		return NULL;
	}

	// Open texture on D3D10 device

	IDXGISurface* dxgiSurface = OpenD3D11TextureOnD3D10Device(p->m_pD2DTexture, pD3D10Device);
	if (!dxgiSurface) {
		return NULL;
	}

	// Obtain mutex for Direct2D

	hr = dxgiSurface->QueryInterface(IID_PPV_ARGS(&p->m_pD2DMutex));
	if (FAILED(hr)) {
		SafeRelease(dxgiSurface);
		return NULL;
	}

	// Create Direct2D render target

	D2D1_RENDER_TARGET_PROPERTIES rtp = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
	hr = pD2DFactory->CreateDxgiSurfaceRenderTarget(dxgiSurface, rtp, &p->m_pD2DTarget);
	if (FAILED(hr)) {
		SafeRelease(dxgiSurface);
		return false;
	}

	SafeRelease(dxgiSurface);

	return p;
}

ID3D11ShaderResourceView* D2DTarget::AcquireSRV()
{
	m_pD3D11Mutex->AcquireSync(0, INFINITE);
	return m_pD2DTextureSRV;
}

void D2DTarget::ReleaseSRV()
{
	m_pD3D11Mutex->ReleaseSync(0);
}

ID2D1RenderTarget* D2DTarget::AcquireTarget()
{
	m_pD2DMutex->AcquireSync(0, INFINITE);
	return m_pD2DTarget;
}

void D2DTarget::ReleaseTarget()
{
	m_pD2DMutex->ReleaseSync(0);
}

}
