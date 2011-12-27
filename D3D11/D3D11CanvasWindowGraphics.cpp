// D3D11CanvasWindowGraphics.cpp
// Nolan Check
// Created 12/26/2011

#include "D3D11CanvasWindowGraphics.hpp"

#include "D3D11Utils.hpp"

namespace D3D11
{

static const DXGI_FORMAT BACKBUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

D3D11CanvasWindowGraphicsPtr D3D11CanvasWindowGraphics::Create(HWND hWnd, ID3D11Device* pDevice, IDXGIFactory1* pDXGIFactory)
{
	D3D11CanvasWindowGraphicsPtr result(new D3D11CanvasWindowGraphics);
	if (!result->CreateInternal(hWnd, pDevice, pDXGIFactory)) {
		return NULL;
	}
	return result;
}

D3D11CanvasWindowGraphics::D3D11CanvasWindowGraphics()
	: m_pSwapChain(NULL)
{ }

D3D11CanvasWindowGraphics::~D3D11CanvasWindowGraphics()
{
	SafeRelease(m_pSwapChain);
}

bool D3D11CanvasWindowGraphics::CreateInternal(HWND hWnd, ID3D11Device* pDevice, IDXGIFactory1* pDXGIFactory)
{
	HRESULT hr;

	DXGI_SWAP_CHAIN_DESC scd = { 0 };
	scd.BufferDesc.Format = BACKBUFFER_FORMAT;
	scd.SampleDesc.Count = 1;
	scd.BufferCount = 1;
	scd.OutputWindow = hWnd;
	scd.Windowed = TRUE;

	hr = pDXGIFactory->CreateSwapChain(pDevice, &scd, &m_pSwapChain);
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

}
