// D3D11Utils.hpp
// Nolan Check
// Created 12/26/2011

#ifndef _D3D11UTILS_HPP
#define _D3D11UTILS_HPP

#include <D3D11.h>

#include <memory>

#include "InputLayout.hpp"

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

class VertexShader;
typedef std::shared_ptr<VertexShader> VertexShaderPtr;
class PixelShader;
typedef std::shared_ptr<PixelShader> PixelShaderPtr;

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

class PixelShader
{

public:

	static PixelShaderPtr Create(ID3D11Device* pDevice,
		const char* src, const char* entryPoint, const char* target);
	~PixelShader();

	ID3D11PixelShader* Get() { return m_pPShader; }

private:

	PixelShader();

	ID3D11PixelShader* m_pPShader;

};

}

#endif
