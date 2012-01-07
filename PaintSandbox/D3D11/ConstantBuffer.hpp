// ConstantBuffer.hpp
// Nolan Check
// Created 1/5/2012

#ifndef _CONSTANTBUFFER_HPP
#define _CONSTANTBUFFER_HPP

#include <D3D11.h>

#include <memory>

namespace D3D11
{

class ConstantBuffer;
typedef std::shared_ptr<ConstantBuffer> ConstantBufferPtr;

class ConstantBuffer
{

public:

	~ConstantBuffer();
	static ConstantBufferPtr Create(ID3D11Device* pDevice, UINT size);

	ID3D11Buffer* Get() { return m_pBuffer; }

private:

	ConstantBuffer();

	ID3D11Buffer* m_pBuffer;

};

}

#endif
