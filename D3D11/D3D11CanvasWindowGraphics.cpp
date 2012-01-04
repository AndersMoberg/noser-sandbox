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

D3D11CanvasWindowGraphicsPtr D3D11CanvasWindowGraphics::Create(HWND hWnd, D3D11DriverPtr driver)
{
	D3D11CanvasWindowGraphicsPtr result(new D3D11CanvasWindowGraphics);
	if (!result->CreateInternal(hWnd, driver)) {
		return NULL;
	}
	return result;
}

D3D11CanvasWindowGraphics::D3D11CanvasWindowGraphics()
	: m_pSwapChain(NULL),
	m_pBackBufferRTV(NULL)
{ }

D3D11CanvasWindowGraphics::~D3D11CanvasWindowGraphics()
{
	SafeRelease(m_pBackBufferRTV);
	SafeRelease(m_pSwapChain);
}

bool D3D11CanvasWindowGraphics::CreateInternal(HWND hWnd, D3D11DriverPtr driver)
{
	HRESULT hr;

	m_hWnd = hWnd;
	m_driver = driver;

	ID3D11Device* pDevice = m_driver->GetD3D11Device();
	IDXGIFactory1* pDXGIFactory = m_driver->GetDXGIFactory();

	// Create swap chain

	DXGI_SWAP_CHAIN_DESC scd = { 0 };
	scd.BufferDesc.Format = BACKBUFFER_FORMAT;
	scd.SampleDesc.Count = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hWnd;
	scd.Windowed = TRUE;
	hr = pDXGIFactory->CreateSwapChain(pDevice, &scd, &m_pSwapChain);
	if (FAILED(hr)) {
		return false;
	}

	if (!CreateSwapChainResources()) {
		return false;
	}

	return true;
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

	hr = pDevice->CreateRenderTargetView(texture, NULL, &m_pBackBufferRTV);
	if (FAILED(hr)) {
		SafeRelease(texture);
		return false;
	}

	SafeRelease(texture);

	return true;
}

void D3D11CanvasWindowGraphics::DestroySwapChainResources()
{
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

	if (m_image)
	{
		// Set up rasterizer
		RECT clientRc;
		GetClientRect(m_hWnd, &clientRc);
		D3D11_VIEWPORT vp = CD3D11_VIEWPORT(
			(FLOAT)clientRc.left,
			(FLOAT)clientRc.top,
			(FLOAT)(clientRc.right - clientRc.left),
			(FLOAT)(clientRc.bottom - clientRc.top));
		pContext->RSSetViewports(1, &vp);

		// Set up output merger
		pContext->OMSetRenderTargets(1, &m_pBackBufferRTV, NULL);
		pContext->OMSetBlendState(m_driver->GetAlphaBlend(), NULL, 0xFFFFFFFF);

		D3D11ImagePtr d3d11Image = std::static_pointer_cast<D3D11Image, DriverImage>(
			m_image->GetDriverImage());
		ID3D11ShaderResourceView* srv = d3d11Image->GetSRV();
		ID3D11SamplerState* ss = m_driver->GetBilinearSampler();

		// Set up pixel shader
		pContext->PSSetShader(m_driver->GetTexturedPixelShader()->Get(), NULL, 0);
		pContext->PSSetShaderResources(0, 1, &srv);
		pContext->PSSetSamplers(0, 1, &ss);

		m_driver->RenderQuad(RectF(-1.0f, 1.0f, 1.0f, -1.0f));

		srv = NULL;
		pContext->PSSetShaderResources(0, 1, &srv);
	}
}

void D3D11CanvasWindowGraphics::Present()
{
	m_pSwapChain->Present(0, 0);
}

}
