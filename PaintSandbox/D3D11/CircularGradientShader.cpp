// CircularGradientShader.cpp
// Nolan Check
// Created 1/2/2012

#include "CircularGradientShader.hpp"

namespace D3D11
{

static const char CIRCULAR_GRADIENT_SHADER[] =
"// PaintSandbox circular gradient pixel shader\n"

"cbuffer cbParams : register(c0)\n"
"{\n"
	"struct\n"
	"{\n"
		"float weight;\n"
	"} Params;\n"
"};\n"

"struct VSOutput\n"
"{\n"
	"float4 pos : SV_Position;\n"
	"float2 tex : TEXCOORD;\n"
"};\n"

"float4 main(VSOutput input) : SV_Target\n"
"{\n"
	"float dist = distance(float2(0.5, 0.5), input.tex);\n"
	"float a = Params.weight * clamp(1.0 - 2.0*dist, 0, 1);\n"
	"return float4(a, a, a, a);\n"
"}\n"
;

struct Params
{
	float weight;
};

CircularGradientShader::CircularGradientShader()
{ }

CircularGradientShaderPtr CircularGradientShader::Create(ID3D11Device* pDevice)
{
	CircularGradientShaderPtr p(new CircularGradientShader);

	p->m_shader = CreatePixelShaderFromCode(pDevice, CIRCULAR_GRADIENT_SHADER, "main", "ps_4_0");
	if (!p->m_shader) {
		return NULL;
	}

	p->m_params = ConstantBuffer::Create(pDevice, sizeof(Params));
	if (!p->m_params) {
		return NULL;
	}

	return p;
}

void CircularGradientShader::Setup(ID3D11DeviceContext* pCtx, float weight)
{
	Params params;
	params.weight = weight;
	pCtx->UpdateSubresource(m_params->Get(), 0, NULL, &weight, sizeof(Params), 0);

	ID3D11Buffer* buf = m_params->Get();

	pCtx->PSSetShader(m_shader->Get(), NULL, 0);
	pCtx->PSSetConstantBuffers(0, 1, &buf);
}

}
