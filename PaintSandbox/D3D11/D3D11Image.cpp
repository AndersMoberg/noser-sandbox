// D3D11Image.cpp
// Nolan Check
// Created 12/27/2011

#include "D3D11Image.hpp"

#include "D3D11DrawToolRenderer.hpp"
#include "D3D11Utils.hpp"

namespace D3D11
{

D3D11Image::D3D11Image()
{ }

D3D11ImagePtr D3D11Image::Create(ID3D11Device* pDevice, DXGI_FORMAT format,
	UINT width, UINT height)
{
	D3D11ImagePtr p(new D3D11Image);

	D3D11_TEXTURE2D_DESC t2dd = CD3D11_TEXTURE2D_DESC(format, width, height,
		1, 1, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);
	p->m_texture = Texture2D::Create(pDevice, t2dd);

	p->m_srv = ShaderResourceView::Create(pDevice, p->m_texture);

	p->m_rtv = RenderTargetView::Create(pDevice, p->m_texture);

	return p;
}

}
