// D3D11Image.cpp
// Nolan Check
// Created 12/27/2011

#include "D3D11Image.hpp"

#include "D3D11Utils.hpp"

namespace D3D11
{

D3D11ImagePtr D3D11Image::Create(ID3D11Device* pDevice, DXGI_FORMAT format,
	UINT width, UINT height)
{
	D3D11ImagePtr result(new D3D11Image);
	if (!result->CreateInternal(pDevice, format, width, height)) {
		return NULL;
	}
	return result;
}

D3D11Image::D3D11Image()
	: m_pTexture(NULL),
	m_pTextureSRV(NULL),
	m_pTextureRTV(NULL)
{ }

D3D11Image::~D3D11Image()
{
	SafeRelease(m_pTextureRTV);
	SafeRelease(m_pTextureSRV);
	SafeRelease(m_pTexture);
}

bool D3D11Image::CreateInternal(ID3D11Device* pDevice, DXGI_FORMAT format,
	UINT width, UINT height)
{
	HRESULT hr;

	D3D11_TEXTURE2D_DESC t2dd = CD3D11_TEXTURE2D_DESC(format, width, height,
		1, 1, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);
	hr = pDevice->CreateTexture2D(&t2dd, NULL, &m_pTexture);
	if (FAILED(hr)) {
		return false;
	}

	hr = pDevice->CreateShaderResourceView(m_pTexture, NULL, &m_pTextureSRV);
	if (FAILED(hr)) {
		return false;
	}

	hr = pDevice->CreateRenderTargetView(m_pTexture, NULL, &m_pTextureRTV);
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

}
