// CircularGradientShader.cpp
// Nolan Check
// Created 1/2/2012

#include "CircularGradientShader.hpp"

namespace D3D11
{

static const char CIRCULAR_GRADIENT_SHADER[] =
"// PaintSandbox circular gradient pixel shader\n"

"struct VSOutput\n"
"{\n"
	"float4 pos : SV_Position;\n"
	"float2 tex : TEXCOORD;\n"
"};\n"

"float4 main(VSOutput input) : SV_Target\n"
"{\n"
	"float dist = distance(float2(0.5, 0.5), input.tex);\n"
	"float a = clamp(1.0 - 2.0*dist, 0, 1);\n"
	"return float4(1, 1, 1, a);\n"
"}\n"
;

CircularGradientShader::CircularGradientShader()
{ }

CircularGradientShaderPtr CircularGradientShader::Create(ID3D11Device* pDevice)
{
	CircularGradientShaderPtr p(new CircularGradientShader);

	p->m_shader = PixelShader::Create(pDevice, CIRCULAR_GRADIENT_SHADER, "main", "ps_4_0");
	if (!p->m_shader) {
		return NULL;
	}

	return p;
}

}
