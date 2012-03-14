// VertexShader.hpp
// Nolan Check
// Created 1/7/2012

#ifndef _VERTEXSHADER_HPP
#define _VERTEXSHADER_HPP

#include <D3D11.h>

#include <memory>

#include "WindowsUtils.hpp"

namespace D3D11
{

class VertexShader;
typedef std::shared_ptr<VertexShader> VertexShaderPtr;

// TODO: Get rid of this class
class VertexShader
{

public:
	
	~VertexShader();
	static VertexShaderPtr Create(ID3D11Device* pDevice,
		const void* pBytecode, SIZE_T bytecodeLength);

	ComPtr<ID3D11VertexShader> Get() { return m_vertexShader; }

private:

	VertexShader();

	ComPtr<ID3D11VertexShader> m_vertexShader;

};

}

#endif
