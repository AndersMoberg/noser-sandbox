// D3D11CanvasWindowGraphics.cpp
// Nolan Check
// Created 12/26/2011

#include "D3D11CanvasWindowGraphics.hpp"

#include "D3D11Driver.hpp"
#include "D3D11Image.hpp"
#include "D3D11Utils.hpp"
#include "WindowsUtils.hpp"

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

	p->m_hWnd = hWnd;
	p->m_driver = driver;
	p->m_camera = camera;

	ID3D11Device* pDevice = p->m_driver->GetD3D11Device();
	IDXGIFactory1* pDXGIFactory = p->m_driver->GetDXGIFactory();
	IDWriteFactory* pDWriteFactory = p->m_driver->GetDWriteFactory();

	// Create text format

	CHECK_HR(pDWriteFactory->CreateTextFormat(
		L"Calibri", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL, 24.0f, L"en-US", &p->m_pTextFormat));

	// Create swap chain

	DXGI_SWAP_CHAIN_DESC scd = { 0 };
	scd.BufferDesc.Format = BACKBUFFER_FORMAT;
	scd.SampleDesc.Count = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hWnd;
	scd.Windowed = TRUE;
	CHECK_HR(pDXGIFactory->CreateSwapChain(pDevice, &scd, &p->m_pSwapChain));

	p->CreateSwapChainResources();

	return p;
}

void D3D11CanvasWindowGraphics::CreateSwapChainResources()
{
	ID3D11Device* pDevice = m_driver->GetD3D11Device();

	// Get back buffer RTV

	ID3D11Texture2D* texture = NULL;
	CHECK_HR(m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&texture)));

	D3D11_TEXTURE2D_DESC t2dd;
	texture->GetDesc(&t2dd);

	CHECK_HR(pDevice->CreateRenderTargetView(texture, NULL, &m_pBackBufferRTV));

	SafeRelease(texture);

	// Create Direct2D render target

	m_d2dTarget = D2DTarget::Create(m_driver->GetD2DFactory(),
		pDevice, m_driver->GetD3D10Device(), t2dd.Width, t2dd.Height);
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

void D3D11CanvasWindowGraphics::SetCanvasImage(CanvasImagePtr image,
	ExtensibleImagePtr extensibleImage)
{
	m_image = image;
	m_extensibleImage = extensibleImage;
}

void D3D11CanvasWindowGraphics::Render()
{
	RECT clientRc;
	GetClientRect(m_hWnd, &clientRc);
	RectF clientRcf((float)clientRc.left, (float)clientRc.top,
		(float)clientRc.right, (float)clientRc.bottom);
	D2D1_RECT_F clientRectf = D2D1::RectF((FLOAT)clientRc.left, (FLOAT)clientRc.top,
		(FLOAT)clientRc.right, (FLOAT)clientRc.bottom);

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
		ID3D11ShaderResourceView* srv = d3d11Image->GetSRV()->Get();
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

	// Create brush for drawing stuff
	ID2D1SolidColorBrush* brush;
	pD2DTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue), &brush);
	
	Matrix3x2f canvasToViewport = m_camera->GetCanvasToViewport(clientRcf);

	// Draw the extensible image's tile structure
	const ExtensibleImage::TileMap& tiles = m_extensibleImage->GetTileMap();
	for (ExtensibleImage::TileMap::const_iterator it = tiles.begin();
		it != tiles.end(); ++it)
	{
		const RectF& canvasRect = it->second->GetCanvasRect();

		ID2D1Factory* d2dFactory;
		pD2DTarget->GetFactory(&d2dFactory);

		ID2D1RectangleGeometry* rectGeom;
		d2dFactory->CreateRectangleGeometry(
			D2D1::RectF(canvasRect.left, canvasRect.top, canvasRect.right, canvasRect.bottom),
			&rectGeom);

		ID2D1TransformedGeometry* transGeom;
		d2dFactory->CreateTransformedGeometry(rectGeom,
			D2D1::Matrix3x2F(canvasToViewport.m11, canvasToViewport.m12,
			canvasToViewport.m21, canvasToViewport.m22,
			canvasToViewport.m31, canvasToViewport.m32),
			&transGeom);

		rectGeom->Release();

		pD2DTarget->DrawGeometry(transGeom, brush);

		transGeom->Release();

		d2dFactory->Release();
	}

	// Draw some text
	RenderPrintf(pD2DTarget, m_pTextFormat, clientRectf, brush,
		L"Welcome to Paint Sandbox!");

	brush->Release();

	pD2DTarget->EndDraw();

	m_d2dTarget->ReleaseTarget();

	// Transfer Direct2D target to display
	//m_driver->RenderD2DTarget(m_d2dTarget);

	// Set up rasterizer
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
