// D3D11Driver.cpp
// Nolan Check
// Created 12/26/2011

#include "D3D11Driver.hpp"

#include "D3D11Image.hpp"
#include "D3D11Utils.hpp"
#include "Geometry.hpp"

namespace D3D11
{

static const char SIMPLE2D_QUAD_VERTEX_SHADER[] =
"// PaintSandbox simple 2D quad vertex shader\n"

"cbuffer cbParams : register(c0)\n"
"{\n"
	"struct\n"
	"{\n"
		"float2 ul;\n"
		"float2 lr;\n"
	"} Params;\n"
"};\n"

"struct VSOutput\n"
"{\n"
	"float4 pos : SV_Position;\n"
	"float2 tex : TEXCOORD;\n"
"};\n"

"VSOutput main(float2 pos : POSITION)\n"
"{\n"
	"VSOutput result;\n"
	"result.pos = float4(lerp(Params.ul, Params.lr, pos), 0, 1);\n"
	"result.tex = pos;\n"
	"return result;\n"
"}\n"
;

struct Simple2DQuadVShaderParams
{
	Vector2f ul;
	Vector2f lr;
};

static const Vector2f SIMPLE2D_QUAD[4] = {
	Vector2f(0.0f, 1.0f), Vector2f(0.0f, 0.0f),
	Vector2f(1.0f, 0.0f), Vector2f(1.0f, 1.0f)
};

static const D3D11_INPUT_ELEMENT_DESC SIMPLE2D_INPUT_LAYOUT[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

static const char TEXTURED_PIXEL_SHADER[] =
"// PaintSandbox textured pixel shader\n"

"struct VSOutput\n"
"{\n"
	"float4 pos : SV_Position;\n"
	"float2 tex : TEXCOORD;\n"
"};\n"

"Texture2D g_Texture : register(t0);\n"
"SamplerState g_Sampler : register(s0);\n"

"float4 main(VSOutput input) : SV_Target\n"
"{\n"
	"return g_Texture.Sample(g_Sampler, input.tex);\n"
"}\n"
;

D3D11DriverPtr D3D11Driver::Create()
{
	D3D11DriverPtr result(new D3D11Driver);
	if (!result->CreateInternal()) {
		return NULL;
	}
	return result;
}

D3D11Driver::D3D11Driver()
	: m_pD3D11Device(NULL),
	m_pD3D11Context(NULL),
	m_pDXGIFactory(NULL),
	m_pBilinearSampler(NULL),
	m_pSimple2DQuad(NULL),
	m_pSimple2DQuadVShaderParams(NULL),
	m_pSimple2DInputLayout(NULL)
{ }

D3D11Driver::~D3D11Driver()
{
	SafeRelease(m_pSimple2DInputLayout);
	SafeRelease(m_pSimple2DQuadVShaderParams);
	SafeRelease(m_pSimple2DQuad);
	SafeRelease(m_pBilinearSampler);
	SafeRelease(m_pDXGIFactory);
	SafeRelease(m_pD3D11Context);
	SafeRelease(m_pD3D11Device);
}

bool D3D11Driver::CreateInternal()
{
	HRESULT hr;

	// Create device

	UINT createFlags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#if _DEBUG
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	hr = D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		createFlags,
		NULL, 0,
		D3D11_SDK_VERSION,
		&m_pD3D11Device,
		NULL,
		&m_pD3D11Context);
	if (FAILED(hr)) {
		return false;
	}

	// Extract DXGI factory from device

	IDXGIDevice* dxgiDevice = NULL;
	hr = m_pD3D11Device->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
	if (FAILED(hr)) {
		return false;
	}

	IDXGIAdapter* dxgiAdapter = NULL;
	hr = dxgiDevice->GetAdapter(&dxgiAdapter);
	if (FAILED(hr)) {
		SafeRelease(dxgiDevice);
		return false;
	}

	SafeRelease(dxgiDevice);

	hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&m_pDXGIFactory));
	if (FAILED(hr)) {
		SafeRelease(dxgiAdapter);
		return false;
	}

	SafeRelease(dxgiAdapter);

	// Create bilinear sampler

	D3D11_SAMPLER_DESC sd = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
	hr = m_pD3D11Device->CreateSamplerState(&sd, &m_pBilinearSampler);
	if (FAILED(hr)) {
		return false;
	}

	// Create simple 2D quad vertex buffer

	D3D11_BUFFER_DESC bd = CD3D11_BUFFER_DESC(sizeof(SIMPLE2D_QUAD),
		D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
	D3D11_SUBRESOURCE_DATA srd = { SIMPLE2D_QUAD, 0, 0 };
	hr = m_pD3D11Device->CreateBuffer(&bd, &srd, &m_pSimple2DQuad);
	if (FAILED(hr)) {
		return false;
	}

	// Create simple 2D quad vertex shader

	m_simple2DQuadVShader = VertexShader::Create(m_pD3D11Device,
		SIMPLE2D_QUAD_VERTEX_SHADER, "main", "vs_4_0",
		SIMPLE2D_INPUT_LAYOUT,
		sizeof(SIMPLE2D_INPUT_LAYOUT)/sizeof(D3D11_INPUT_ELEMENT_DESC),
		&m_pSimple2DInputLayout);
	if (!m_simple2DQuadVShader) {
		return false;
	}

	// Create simple 2d quad vertex shader parameters buffer

	bd = CD3D11_BUFFER_DESC(sizeof(Simple2DQuadVShaderParams),
		D3D11_BIND_CONSTANT_BUFFER);
	hr = m_pD3D11Device->CreateBuffer(&bd, NULL, &m_pSimple2DQuadVShaderParams);
	if (FAILED(hr)) {
		return false;
	}

	// Create textured pixel shader

	m_texturedPShader = PixelShader::Create(m_pD3D11Device,
		TEXTURED_PIXEL_SHADER, "main", "ps_4_0");
	if (!m_texturedPShader) {
		return false;
	}

	// Create circular gradient shader

	m_circularGradientShader = CircularGradientShader::Create(m_pD3D11Device);
	if (!m_circularGradientShader) {
		return false;
	}

	return true;
}

CanvasWindowGraphicsPtr D3D11Driver::CreateWindowGraphics(HWND hWnd)
{
	return D3D11CanvasWindowGraphics::Create(hWnd, shared_from_this());
}

DriverImagePtr D3D11Driver::CreateImage(int width, int height)
{
	return D3D11Image::Create(m_pD3D11Device, DXGI_FORMAT_R16G16B16A16_FLOAT, width, height);
}

void D3D11Driver::RenderQuad(const RectF& rc)
{
	// Load shader parameters
	Simple2DQuadVShaderParams params = { rc.UpperLeft(), rc.LowerRight() };
	m_pD3D11Context->UpdateSubresource(m_pSimple2DQuadVShaderParams, 0, NULL, &params, 0, 0);

	// Set up input assembler
	m_pD3D11Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	m_pD3D11Context->IASetInputLayout(m_pSimple2DInputLayout);
	UINT stride = sizeof(Vector2f);
	UINT offset = 0;
	m_pD3D11Context->IASetVertexBuffers(0, 1, &m_pSimple2DQuad, &stride, &offset);

	// Set up vertex shader
	m_pD3D11Context->VSSetShader(m_simple2DQuadVShader->Get(), NULL, 0);
	m_pD3D11Context->VSSetConstantBuffers(0, 1, &m_pSimple2DQuadVShaderParams);

	// Draw!
	m_pD3D11Context->Draw(4, 0);
};

}
