// D3D11Image.cpp
// Nolan Check
// Created 12/27/2011

#include "D3D11Image.hpp"

#include "D3D11DrawToolRenderer.hpp"
#include "D3D11Utils.hpp"

namespace D3D11
{

D3D11Image::D3D11Image()
	: m_pTextureRTV(NULL)
{ }

D3D11Image::~D3D11Image()
{
	SafeRelease(m_pTextureRTV);
}

D3D11ImagePtr D3D11Image::Create(ID3D11Device* pDevice, DXGI_FORMAT format,
	UINT width, UINT height)
{
	D3D11ImagePtr p(new D3D11Image);

	HRESULT hr;

	D3D11_TEXTURE2D_DESC t2dd = CD3D11_TEXTURE2D_DESC(format, width, height,
		1, 1, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);
	p->m_texture = Texture2D::Create(pDevice, t2dd);
	if (!p->m_texture) {
		return NULL;
	}

	p->m_srv = ShaderResourceView::Create(pDevice, p->m_texture);
	if (!p->m_srv) {
		return NULL;
	}

	hr = pDevice->CreateRenderTargetView(p->m_texture->Get(), NULL, &p->m_pTextureRTV);
	if (FAILED(hr)) {
		return NULL;
	}

	return p;
}

}
