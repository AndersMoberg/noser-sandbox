// D3D11Driver.cpp
// Nolan Check
// Created 12/26/2011

#include "D3D11Driver.hpp"

#include "D3D11DrawToolRenderer.hpp"
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
		"float2 mat0;\n"
		"float2 mat1;\n"
		"float2 mat2;\n"
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
	"float2 canvasPos = lerp(Params.ul, Params.lr, pos);\n"
	"float2 clipPos = float2(dot(Params.mat0, canvasPos), dot(Params.mat1, canvasPos)) + Params.mat2;\n"
	"result.pos = float4(clipPos, 0, 1);\n"
	"result.tex = pos;\n"
	"return result;\n"
"}\n"
;

struct Simple2DQuadVShaderParams
{
	Matrix3x2f mat;
	Vector2f ul;
	Vector2f lr;
};

static const Vector2f SIMPLE2D_QUAD[4] = {
	Vector2f(0.0f, 0.0f), Vector2f(1.0f, 0.0f),
	Vector2f(0.0f, 1.0f), Vector2f(1.0f, 1.0f)
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

D3D11Driver::D3D11Driver()
	: m_pD3D11Device(NULL),
	m_pD3D11Context(NULL),
	m_pDXGIFactory(NULL),
	m_pD3D10Device(NULL),
	m_pD2DFactory(NULL),
	m_pDWriteFactory(NULL)
{ }

D3D11Driver::~D3D11Driver()
{
	SafeRelease(m_pDWriteFactory);
	SafeRelease(m_pD2DFactory);
	SafeRelease(m_pD3D10Device);
	SafeRelease(m_pDXGIFactory);
	SafeRelease(m_pD3D11Context);
	SafeRelease(m_pD3D11Device);
}

D3D11DriverPtr D3D11Driver::Create()
{
	D3D11DriverPtr p(new D3D11Driver);

	HRESULT hr;

	// Create device

	UINT createFlags = D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT;
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
		&p->m_pD3D11Device,
		NULL,
		&p->m_pD3D11Context);
	if (FAILED(hr)) {
		return NULL;
	}

	// Get DXGI adapter from D3D11 device

	IDXGIAdapter* dxgiAdapter = GetDXGIAdapterFromD3D11Device(p->m_pD3D11Device);
	if (!dxgiAdapter) {
		return NULL;
	}

	// Get DXGI factory from adapter

	hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&p->m_pDXGIFactory));
	if (FAILED(hr)) {
		SafeRelease(dxgiAdapter);
		return NULL;
	}

	// Create D3D10.1 device using the same adapter chosen by D3D11CreateDevice

	createFlags = D3D10_CREATE_DEVICE_SINGLETHREADED | D3D10_CREATE_DEVICE_BGRA_SUPPORT;
#if _DEBUG
	// XXX: This is commented out because Direct2D causes D3D10 to spam debug output
	//createFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif
	// TODO: Will feature level 10.1 work for most people? I think Direct2D can
	// work with any feature level.
	hr = D3D10CreateDevice1(dxgiAdapter, D3D10_DRIVER_TYPE_HARDWARE, NULL,
		createFlags, D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &p->m_pD3D10Device);
	if (FAILED(hr)) {
		SafeRelease(dxgiAdapter);
		return NULL;
	}

	SafeRelease(dxgiAdapter);

	// Create Direct2D factory

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &p->m_pD2DFactory);
	if (FAILED(hr)) {
		return NULL;
	}

	// Create DirectWrite factory

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory), (IUnknown**)&p->m_pDWriteFactory);
	if (FAILED(hr)) {
		return NULL;
	}

	// Create blend state for Porter-Duff "over" operation

	D3D11_BLEND_DESC bld = CD3D11_BLEND_DESC(CD3D11_DEFAULT());
	bld.RenderTarget[0].BlendEnable = TRUE;
	bld.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	bld.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bld.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bld.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bld.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	bld.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	p->m_overBlend = BlendState::Create(p->m_pD3D11Device, bld);
	if (!p->m_overBlend) {
		return NULL;
	}

	// Create bilinear sampler

	D3D11_SAMPLER_DESC sd = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
	p->m_bilinearSampler = SamplerState::Create(p->m_pD3D11Device, sd);
	if (!p->m_bilinearSampler) {
		return NULL;
	}

	// Create simple 2D quad vertex buffer

	D3D11_BUFFER_DESC bd = CD3D11_BUFFER_DESC(sizeof(SIMPLE2D_QUAD),
		D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
	D3D11_SUBRESOURCE_DATA srd = { SIMPLE2D_QUAD, 0, 0 };
	p->m_simple2DQuad = Buffer::Create(p->m_pD3D11Device, bd, srd);
	if (!p->m_simple2DQuad) {
		return NULL;
	}

	// Create simple 2D quad vertex shader

	p->m_simple2DQuadVShader = CreateVertexShaderFromCode(p->m_pD3D11Device,
		SIMPLE2D_QUAD_VERTEX_SHADER, "main", "vs_4_0",
		SIMPLE2D_INPUT_LAYOUT,
		sizeof(SIMPLE2D_INPUT_LAYOUT)/sizeof(D3D11_INPUT_ELEMENT_DESC),
		&p->m_simple2DInputLayout);
	if (!p->m_simple2DQuadVShader) {
		return NULL;
	}

	// Create simple 2d quad vertex shader parameters buffer

	p->m_simple2DQuadParams = ConstantBuffer::Create(p->m_pD3D11Device,
		sizeof(Simple2DQuadVShaderParams));
	if (!p->m_simple2DQuadParams) {
		return NULL;
	}

	// Create textured pixel shader

	p->m_texturedPShader = CreatePixelShaderFromCode(p->m_pD3D11Device,
		TEXTURED_PIXEL_SHADER, "main", "ps_4_0");
	if (!p->m_texturedPShader) {
		return NULL;
	}

	// Create circular gradient shader

	p->m_circularGradientShader = CircularGradientShader::Create(p->m_pD3D11Device);
	if (!p->m_circularGradientShader) {
		return NULL;
	}

	return p;
}

CanvasWindowGraphicsPtr D3D11Driver::CreateWindowGraphics(HWND hWnd, CameraPtr camera)
{
	return D3D11CanvasWindowGraphics::Create(hWnd, shared_from_this(), camera);
}

DriverImagePtr D3D11Driver::CreateImage(int width, int height)
{
	return D3D11Image::Create(m_pD3D11Device, DXGI_FORMAT_R16G16B16A16_FLOAT, width, height);
}

DrawToolRendererPtr D3D11Driver::CreateDrawToolRenderer(CanvasImagePtr image)
{
	return D3D11DrawToolRenderer::Create(shared_from_this(), image);
}

void D3D11Driver::RenderQuad(const Matrix3x2f& mat, const RectF& rc)
{
	// Load shader parameters
	Simple2DQuadVShaderParams params;
	params.mat = mat;
	params.ul = rc.UpperLeft();
	params.lr = rc.LowerRight();
	m_pD3D11Context->UpdateSubresource(m_simple2DQuadParams->Get(), 0, NULL, &params, 0, 0);

	// Set up input assembler
	m_pD3D11Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	m_pD3D11Context->IASetInputLayout(m_simple2DInputLayout->Get());
	UINT stride = sizeof(Vector2f);
	UINT offset = 0;
	ID3D11Buffer* buf = m_simple2DQuad->Get();
	m_pD3D11Context->IASetVertexBuffers(0, 1, &buf, &stride, &offset);

	// Set up vertex shader
	m_pD3D11Context->VSSetShader(m_simple2DQuadVShader->Get(), NULL, 0);
	buf = m_simple2DQuadParams->Get();
	m_pD3D11Context->VSSetConstantBuffers(0, 1, &buf);

	// Draw!
	m_pD3D11Context->Draw(4, 0);
};

void D3D11Driver::RenderQuadToCanvas(CanvasImagePtr canvas, const RectF& rc)
{
	D3D11ImagePtr image = std::static_pointer_cast<D3D11Image, DriverImage>(
		canvas->GetDriverImage());
	ID3D11RenderTargetView* rtv = image->GetRTV();

	// Set up output merger
	m_pD3D11Context->OMSetRenderTargets(1, &rtv, NULL);

	Matrix3x2f mat = Matrix3x2f::RectLerp(canvas->GetCanvasRect(),
		RectF(-1.0f, 1.0f, 1.0f, -1.0f));
	RenderQuad(mat, rc);
}

void D3D11Driver::RenderD2DTarget(D2DTargetPtr d2dTarget)
{
	// TODO: Implement
}

}
