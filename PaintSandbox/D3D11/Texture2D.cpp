// Texture2D.cpp
// Nolan Check
// Created 1/11/2012

#include "Texture2D.hpp"

#include "D3D11Utils.hpp"

namespace D3D11
{

Texture2D::Texture2D()
	: m_pTexture(NULL)
{ }

Texture2D::~Texture2D()
{
	SafeRelease(m_pTexture);
}

Texture2DPtr Texture2D::Create(ID3D11Device* pDevice, const D3D11_TEXTURE2D_DESC& t2dd)
{
	Texture2DPtr p(new Texture2D);

	HRESULT hr = pDevice->CreateTexture2D(&t2dd, NULL, &p->m_pTexture);
	if (FAILED(hr)) {
		return NULL;
	}

	return p;
}

}
