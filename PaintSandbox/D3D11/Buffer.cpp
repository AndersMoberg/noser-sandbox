// Buffer.cpp
// Nolan Check
// Created 1/6/2012

#include "Buffer.hpp"

#include "D3D11Utils.hpp"
#include "WindowsUtils.hpp"

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

	p->CreateInternal(pDevice, bd, &srd);

	return p;
}

void Buffer::CreateInternal(ID3D11Device* pDevice, const D3D11_BUFFER_DESC& bd,
	const D3D11_SUBRESOURCE_DATA* psrd)
{
	CHECK_HR(pDevice->CreateBuffer(&bd, psrd, &m_pBuffer));
}

}
