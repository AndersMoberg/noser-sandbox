// D3D11CanvasWindowGraphics.cpp
// Nolan Check
// Created 12/26/2011

#include "D3D11CanvasWindowGraphics.hpp"

#include "D3D11Driver.hpp"
#include "D3D11Image.hpp"
#include "D3D11Utils.hpp"

namespace D3D11
{

static const DXGI_FORMAT BACKBUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

D3D11CanvasWindowGraphics::D3D11CanvasWindowGraphics()
	: m_pSwapChain(NULL),
	m_pBackBufferRTV(NULL),
	m_pTextFormat(NULL)
{ }

D3D11CanvasWindowGraphics::~D3D11CanvasWindowGraphics()
{
	SafeRelease(m_pTextFormat);
	SafeRelease(m_pBackBufferRTV);
	SafeRelease(m_pSwapChain);
}

D3D11CanvasWindowGraphicsPtr D3D11CanvasWindowGraphics::Create(
	HWND hWnd, D3D11DriverPtr driver, CameraPtr camera)
{
	D3D11CanvasWindowGraphicsPtr p(new D3D11CanvasWindowGraphics);

	HRESULT hr;

	p->m_hWnd = hWnd;
	p->m_driver = driver;
	p->m_camera = camera;

	ID3D11Device* pDevice = p->m_driver->GetD3D11Device();
	IDXGIFactory1* pDXGIFactory = p->m_driver->GetDXGIFactory();
	IDWriteFactory* pDWriteFactory = p->m_driver->GetDWriteFactory();

	// Create text format

	hr = pDWriteFactory->CreateTextFormat(
		L"Calibri", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL, 24.0f, L"en-US", &p->m_pTextFormat);
	if (FAILED(hr)) {
		return false;
	}

	// Create swap chain

	DXGI_SWAP_CHAIN_DESC scd = { 0 };
	scd.BufferDesc.Format = BACKBUFFER_FORMAT;
	scd.SampleDesc.Count = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hWnd;
	scd.Windowed = TRUE;
	hr = pDXGIFactory->CreateSwapChain(pDevice, &scd, &p->m_pSwapChain);
	if (FAILED(hr)) {
		return NULL;
	}

	if (!p->CreateSwapChainResources()) {
		return NULL;
	}

	return p;
}

bool D3D11CanvasWindowGraphics::CreateSwapChainResources()
{
	HRESULT hr;

	ID3D11Device* pDevice = m_driver->GetD3D11Device();

	// Get back buffer RTV

	ID3D11Texture2D* texture = NULL;
	hr = m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&texture));
	if (FAILED(hr)) {
		return false;
	}

	D3D11_TEXTURE2D_DESC t2dd;
	texture->GetDesc(&t2dd);

	hr = pDevice->CreateRenderTargetView(texture, NULL, &m_pBackBufferRTV);
	if (FAILED(hr)) {
		SafeRelease(texture);
		return false;
	}

	SafeRelease(texture);

	// Create Direct2D render target

	m_d2dTarget = D2DTarget::Create(m_driver->GetD2DFactory(),
		pDevice, m_driver->GetD3D10Device(), t2dd.Width, t2dd.Height);
	if (!m_d2dTarget) {
		return false;
	}

	return true;
}

void D3D11CanvasWindowGraphics::DestroySwapChainResources()
{
	m_d2dTarget.reset();
	SafeRelease(m_pBackBufferRTV);
}

void D3D11CanvasWindowGraphics::OnWMSize()
{
	DestroySwapChainResources();
	m_pSwapChain->ResizeBuffers(1, 0, 0, BACKBUFFER_FORMAT, 0);
	CreateSwapChainResources();
}

void D3D11CanvasWindowGraphics::OnWMPaint()
{
	PAINTSTRUCT ps;
	BeginPaint(m_hWnd, &ps);

	Render();
	Present();

	EndPaint(m_hWnd, &ps);
}

void D3D11CanvasWindowGraphics::SetCanvasImage(CanvasImagePtr image)
{
	m_image = image;
}

void D3D11CanvasWindowGraphics::Render()
{
	ID3D11DeviceContext* pContext = m_driver->GetD3D11Context();

	// Clear to black
	static const float BG_COLOR[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	pContext->ClearRenderTargetView(m_pBackBufferRTV, BG_COLOR);

	// TODO: CLEAN UP THIS MESS!!!

	if (m_image)
	{
		// Set up rasterizer
		RECT clientRc;
		GetClientRect(m_hWnd, &clientRc);
		RectF clientRcf((float)clientRc.left, (float)clientRc.top,
			(float)clientRc.right, (float)clientRc.bottom);
		D3D11_VIEWPORT vp = CD3D11_VIEWPORT(
			clientRcf.left,
			clientRcf.top,
			clientRcf.right - clientRcf.left,
			clientRcf.bottom - clientRcf.top);
		pContext->RSSetViewports(1, &vp);

		// Set up output merger
		pContext->OMSetRenderTargets(1, &m_pBackBufferRTV, NULL);
		pContext->OMSetBlendState(m_driver->GetOverBlend()->Get(), NULL, 0xFFFFFFFF);

		D3D11ImagePtr d3d11Image = std::static_pointer_cast<D3D11Image, DriverImage>(
			m_image->GetDriverImage());
		ID3D11ShaderResourceView* srv = d3d11Image->GetSRV();
		ID3D11SamplerState* ss = m_driver->GetBilinearSampler()->Get();

		// Set up pixel shader
		pContext->PSSetShader(m_driver->GetTexturedPixelShader()->Get(), NULL, 0);
		pContext->PSSetShaderResources(0, 1, &srv);
		pContext->PSSetSamplers(0, 1, &ss);

		m_driver->RenderQuad(m_camera->GetCanvasToClip(clientRcf),
			m_image->GetCanvasRect());

		srv = NULL;
		pContext->PSSetShaderResources(0, 1, &srv);
	}

	// Render something into Direct2D target

	ID2D1RenderTarget* pD2DTarget = m_d2dTarget->AcquireTarget();

	pD2DTarget->BeginDraw();

	// Clear to transparent black
	pD2DTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black, 0.0f));

	// Create brush for drawing text
	ID2D1SolidColorBrush* brush;
	pD2DTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Coral), &brush);

	// Draw some text
	RECT clientRc;
	GetClientRect(m_hWnd, &clientRc);
	D2D1_RECT_F clientRectf = D2D1::RectF((FLOAT)clientRc.left, (FLOAT)clientRc.top,
		(FLOAT)clientRc.right, (FLOAT)clientRc.bottom);
	RenderPrintf(pD2DTarget, m_pTextFormat, clientRectf, brush,
		L"Welcome to Paint Sandbox!");

	brush->Release();

	pD2DTarget->EndDraw();

	m_d2dTarget->ReleaseTarget();

	// Transfer Direct2D target to display
	//m_driver->RenderD2DTarget(m_d2dTarget);

	// Set up rasterizer
	RectF clientRcf((float)clientRc.left, (float)clientRc.top,
		(float)clientRc.right, (float)clientRc.bottom);
	D3D11_VIEWPORT vp = CD3D11_VIEWPORT(
		clientRcf.left,
		clientRcf.top,
		clientRcf.right - clientRcf.left,
		clientRcf.bottom - clientRcf.top);
	pContext->RSSetViewports(1, &vp);

	// Set up output merger
	pContext->OMSetRenderTargets(1, &m_pBackBufferRTV, NULL);
	pContext->OMSetBlendState(m_driver->GetOverBlend()->Get(), NULL, 0xFFFFFFFF);

	ID3D11ShaderResourceView* srv = m_d2dTarget->AcquireSRV();
	ID3D11SamplerState* ss = m_driver->GetBilinearSampler()->Get();

	// Set up pixel shader
	pContext->PSSetShader(m_driver->GetTexturedPixelShader()->Get(), NULL, 0);
	pContext->PSSetShaderResources(0, 1, &srv);
	pContext->PSSetSamplers(0, 1, &ss);

	m_driver->RenderQuad(Matrix3x2f::IDENTITY, RectF(-1.0f, 1.0f, 1.0f, -1.0f));

	srv = NULL;
	pContext->PSSetShaderResources(0, 1, &srv);

	m_d2dTarget->ReleaseSRV();
}

void D3D11CanvasWindowGraphics::Present()
{
	m_pSwapChain->Present(0, 0);
}

void D3D11CanvasWindowGraphics::RenderPrintf(ID2D1RenderTarget* pD2DTarget,
	IDWriteTextFormat* textFormat, const D2D1_RECT_F& layoutRect,
	ID2D1Brush* defaultForegroundBrush,
	LPCWSTR msg, ...)
{
	va_list args = NULL;
	va_start(args, msg);

	int len = _vscwprintf(msg, args);

	WCHAR* buf = new WCHAR[len + 1];
	vswprintf_s(buf, len + 1, msg, args);

	va_end(args);

	pD2DTarget->DrawTextW(buf, len, textFormat, layoutRect, defaultForegroundBrush);

	delete[] buf;
}

}
