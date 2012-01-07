// D3D11Utils.hpp
// Nolan Check
// Created 12/26/2011

#ifndef _D3D11UTILS_HPP
#define _D3D11UTILS_HPP

#include <D3D11.h>

#include <memory>

#include "InputLayout.hpp"
#include "PixelShader.hpp"

namespace D3D11
{

template<class Interface>
inline void SafeRelease(Interface*& pi)
{
	if (pi)
	{
		pi->Release();
		pi = NULL;
	}
}

PixelShaderPtr CreatePixelShaderFromCode(ID3D11Device* pDevice,
	const char* src, const char* entryPoint, const char* target);

class VertexShader;
typedef std::shared_ptr<VertexShader> VertexShaderPtr;

class VertexShader
{

public:

	static VertexShaderPtr Create(ID3D11Device* pDevice,
		const char* src, const char* entryPoint, const char* target,
		const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs = NULL, UINT numElements = 0,
		InputLayoutPtr* ppInputLayout = NULL);
	~VertexShader();

	ID3D11VertexShader* Get() { return m_pVShader; }

private:

	VertexShader();

	ID3D11VertexShader* m_pVShader;

};

}

#endif
