// D2DTarget.cpp
// Nolan Check
// Created 1/10/2012

#include "D2DTarget.hpp"

#include "D3D11Utils.hpp"
#include "WindowsUtils.hpp"

namespace D3D11
{

static const DXGI_FORMAT D2DTEXTURE_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

D2DTarget::D2DTarget()
	: m_pD3D11Mutex(NULL),
	m_pD2DMutex(NULL),
	m_pD2DTarget(NULL)
{ }

D2DTarget::~D2DTarget()
{
	SafeRelease(m_pD2DTarget);
	SafeRelease(m_pD2DMutex);
	SafeRelease(m_pD3D11Mutex);
}

D2DTargetPtr D2DTarget::Create(ID2D1Factory* pD2DFactory,
	ID3D11Device* pD3D11Device, ID3D10Device1* pD3D10Device,
	UINT width, UINT height)
{
	D2DTargetPtr p(new D2DTarget);

	// Create texture for rendering into

	D3D11_TEXTURE2D_DESC t2dd = CD3D11_TEXTURE2D_DESC(D2DTEXTURE_FORMAT,
		width, height, 1, 1, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		D3D11_USAGE_DEFAULT, 0, 1, 0, D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX);
	p->m_d2dTexture = Texture2D::Create(pD3D11Device, t2dd);

	// Create SRV for texture

	p->m_d2dTextureSRV = ShaderResourceView::Create(pD3D11Device, p->m_d2dTexture);

	// Obtain mutex for D3D11

	CHECK_HR(p->m_d2dTexture->Get()->QueryInterface(IID_PPV_ARGS(&p->m_pD3D11Mutex)));

	// Open texture on D3D10 device

	IDXGISurface* dxgiSurface = OpenD3D11TextureOnD3D10Device(p->m_d2dTexture->Get(), pD3D10Device);

	// Obtain mutex for Direct2D

	CHECK_HR(dxgiSurface->QueryInterface(IID_PPV_ARGS(&p->m_pD2DMutex)));

	// Create Direct2D render target

	D2D1_RENDER_TARGET_PROPERTIES rtp = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
	CHECK_HR(pD2DFactory->CreateDxgiSurfaceRenderTarget(dxgiSurface, rtp, &p->m_pD2DTarget));

	SafeRelease(dxgiSurface);

	return p;
}

ID3D11ShaderResourceView* D2DTarget::AcquireSRV()
{
	CHECK_HR(m_pD3D11Mutex->AcquireSync(0, INFINITE));
	return m_d2dTextureSRV->Get();
}

void D2DTarget::ReleaseSRV()
{
	CHECK_HR(m_pD3D11Mutex->ReleaseSync(0));
}

ID2D1RenderTarget* D2DTarget::AcquireTarget()
{
	CHECK_HR(m_pD2DMutex->AcquireSync(0, INFINITE));
	return m_pD2DTarget;
}

void D2DTarget::ReleaseTarget()
{
	CHECK_HR(m_pD2DMutex->ReleaseSync(0));
}

}