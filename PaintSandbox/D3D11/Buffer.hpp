// Buffer.hpp
// Nolan Check
// Created 1/6/2012

#ifndef _BUFFER_HPP
#define _BUFFER_HPP

#include <D3D11.h>

#include <memory>

namespace D3D11
{

class Buffer;
typedef std::shared_ptr<Buffer> BufferPtr;

class Buffer
{

public:

	~Buffer();
	static BufferPtr Create(ID3D11Device* pDevice, const D3D11_BUFFER_DESC& bd);
	static BufferPtr Create(ID3D11Device* pDevice, const D3D11_BUFFER_DESC& bd,
		const D3D11_SUBRESOURCE_DATA& srd);

	ID3D11Buffer* Get() { return m_pBuffer; }

private:

	Buffer();
	void CreateInternal(ID3D11Device* pDevice, const D3D11_BUFFER_DESC& bd,
		const D3D11_SUBRESOURCE_DATA* psrd);

	ID3D11Buffer* m_pBuffer;

};

}

#endif
