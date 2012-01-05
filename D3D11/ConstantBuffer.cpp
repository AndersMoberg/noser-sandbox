// ConstantBuffer.cpp
// Nolan Check
// Created 1/5/2012

#include "ConstantBuffer.hpp"

#include "D3D11Utils.hpp"

namespace D3D11
{

ConstantBuffer::ConstantBuffer()
	: m_pBuffer(NULL)
{ }

ConstantBuffer::~ConstantBuffer()
{
	SafeRelease(m_pBuffer);
}

ConstantBufferPtr ConstantBuffer::Create(ID3D11Device* pDevice, UINT size)
{
	ConstantBufferPtr p(new ConstantBuffer);

	HRESULT hr;

	D3D11_BUFFER_DESC bd = CD3D11_BUFFER_DESC((size+15)&~15,
		D3D11_BIND_CONSTANT_BUFFER);
	hr = pDevice->CreateBuffer(&bd, NULL, &p->m_pBuffer);
	if (FAILED(hr)) {
		return NULL;
	}

	return p;
}

}
