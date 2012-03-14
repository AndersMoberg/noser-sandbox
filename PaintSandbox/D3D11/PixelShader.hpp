// PixelShader.hpp
// Nolan Check
// Created 1/6/2012

#ifndef _PIXELSHADER_HPP
#define _PIXELSHADER_HPP

#include <D3D11.h>

#include <memory>

#include "WindowsUtils.hpp"

namespace D3D11
{

class PixelShader;
typedef std::shared_ptr<PixelShader> PixelShaderPtr;

// TODO: Get rid of this class
class PixelShader
{

public:
	
	~PixelShader();
	static PixelShaderPtr Create(ID3D11Device* pDevice,
		const void* pBytecode, SIZE_T bytecodeLength);

	ComPtr<ID3D11PixelShader> Get() { return m_pixelShader; }

private:

	PixelShader();

	ComPtr<ID3D11PixelShader> m_pixelShader;

};

}

#endif
