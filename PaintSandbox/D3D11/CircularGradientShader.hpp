// CircularGradientShader.hpp
// Nolan Check
// Created 1/2/2012

#ifndef _CIRCULARGRADIENTSHADER_HPP
#define _CIRCULARGRADIENTSHADER_HPP

#include <memory>

#include "D3D11Utils.hpp"
#include "ConstantBuffer.hpp"

namespace D3D11
{

class CircularGradientShader;
typedef std::shared_ptr<CircularGradientShader> CircularGradientShaderPtr;

class CircularGradientShader
{

public:

	static CircularGradientShaderPtr Create(ID3D11Device* pDevice);

	void Setup(ID3D11DeviceContext* pCtx, float weight);

private:

	CircularGradientShader();

	ComPtr<ID3D11PixelShader> m_shader;
	ConstantBufferPtr m_params;

};

}

#endif
