// CircularGradientShader.hpp
// Nolan Check
// Created 1/2/2012

#ifndef _CIRCULARGRADIENTSHADER_HPP
#define _CIRCULARGRADIENTSHADER_HPP

#include <memory>

#include "D3D11Utils.hpp"

namespace D3D11
{

class CircularGradientShader;
typedef std::shared_ptr<CircularGradientShader> CircularGradientShaderPtr;

class CircularGradientShader
{

public:

	static CircularGradientShaderPtr Create(ID3D11Device* pDevice);

	ID3D11PixelShader* Get() { return m_shader->Get(); }

private:

	CircularGradientShader();
	bool CreateInternal(ID3D11Device* pDevice);

	PixelShaderPtr m_shader;

};

}

#endif
