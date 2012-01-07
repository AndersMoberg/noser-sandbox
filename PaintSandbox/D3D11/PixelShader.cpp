// PixelShader.cpp
// Nolan Check
// Created 1/6/2012

#include "PixelShader.hpp"

#include "D3D11Utils.hpp"

namespace D3D11
{

PixelShader::PixelShader()
	: m_pPixelShader(NULL)
{ }

PixelShader::~PixelShader()
{
	SafeRelease(m_pPixelShader);
}

PixelShaderPtr PixelShader::Create(ID3D11Device* pDevice,
	const void* pBytecode, SIZE_T bytecodeLength)
{
	PixelShaderPtr p(new PixelShader);

	HRESULT hr = pDevice->CreatePixelShader(pBytecode, bytecodeLength,
		NULL, &p->m_pPixelShader);
	if (FAILED(hr)) {
		return NULL;
	}

	return p;
}

}
