// PixelShader.hpp
// Nolan Check
// Created 1/6/2012

#ifndef _PIXELSHADER_HPP
#define _PIXELSHADER_HPP

#include <D3D11.h>

#include <memory>

namespace D3D11
{

class PixelShader;
typedef std::shared_ptr<PixelShader> PixelShaderPtr;

class PixelShader
{

public:
	
	~PixelShader();
	static PixelShaderPtr Create(ID3D11Device* pDevice,
		const void* pBytecode, SIZE_T bytecodeLength);

	ID3D11PixelShader* Get() { return m_pPixelShader; }

private:

	PixelShader();

	ID3D11PixelShader* m_pPixelShader;

};

}

#endif
