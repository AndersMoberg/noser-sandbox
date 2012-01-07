// VertexShader.hpp
// Nolan Check
// Created 1/7/2012

#ifndef _VERTEXSHADER_HPP
#define _VERTEXSHADER_HPP

#include <D3D11.h>

#include <memory>

namespace D3D11
{

class VertexShader;
typedef std::shared_ptr<VertexShader> VertexShaderPtr;

class VertexShader
{

public:
	
	~VertexShader();
	static VertexShaderPtr Create(ID3D11Device* pDevice,
		const void* pBytecode, SIZE_T bytecodeLength);

	ID3D11VertexShader* Get() { return m_pVertexShader; }

private:

	VertexShader();

	ID3D11VertexShader* m_pVertexShader;

};

}

#endif
