// ShaderResourceView.cpp
// Nolan Check
// Created 1/11/2012

#include "ShaderResourceView.hpp"

#include "D3D11Utils.hpp"
#include "WindowsUtils.hpp"

namespace D3D11
{

ShaderResourceView::ShaderResourceView()
	: m_pSRV(NULL)
{ }

ShaderResourceView::~ShaderResourceView()
{
	SafeRelease(m_pSRV);
}

ShaderResourceViewPtr ShaderResourceView::Create(ID3D11Device* pDevice, Texture2DPtr texture)
{
	ShaderResourceViewPtr p(new ShaderResourceView);

	CHECK_HR(pDevice->CreateShaderResourceView(texture->Get(), NULL, &p->m_pSRV));

	return p;
}

}
