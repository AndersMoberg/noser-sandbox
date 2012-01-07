// VertexShader.cpp
// Nolan Check
// Created 1/7/2012

#include "VertexShader.hpp"

#include "D3D11Utils.hpp"

namespace D3D11
{

VertexShader::VertexShader()
	: m_pVertexShader(NULL)
{ }

VertexShader::~VertexShader()
{
	SafeRelease(m_pVertexShader);
}

VertexShaderPtr VertexShader::Create(ID3D11Device* pDevice,
	const void* pBytecode, SIZE_T bytecodeLength)
{
	VertexShaderPtr p(new VertexShader);
	
	HRESULT hr = pDevice->CreateVertexShader(pBytecode,
		bytecodeLength, NULL, &p->m_pVertexShader);
	if (FAILED(hr)) {
		return NULL;
	}

	return p;
}

}
