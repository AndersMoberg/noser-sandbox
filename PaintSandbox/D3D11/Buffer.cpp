// Buffer.cpp
// Nolan Check
// Created 1/6/2012

#include "Buffer.hpp"

#include "D3D11Utils.hpp"

namespace D3D11
{

Buffer::Buffer()
	: m_pBuffer(NULL)
{ }

Buffer::~Buffer()
{
	SafeRelease(m_pBuffer);
}

BufferPtr Buffer::Create(ID3D11Device* pDevice, const D3D11_BUFFER_DESC& bd,
	const D3D11_SUBRESOURCE_DATA& srd)
{
	BufferPtr p(new Buffer);

	if (!p->CreateInternal(pDevice, bd, &srd)) {
		return NULL;
	}

	return p;
}

bool Buffer::CreateInternal(ID3D11Device* pDevice, const D3D11_BUFFER_DESC& bd,
	const D3D11_SUBRESOURCE_DATA* psrd)
{
	HRESULT hr = pDevice->CreateBuffer(&bd, psrd, &m_pBuffer);
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

}
