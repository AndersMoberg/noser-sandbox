// D3D11CanvasWindowGraphics.hpp
// Nolan Check
// Created 12/26/2011

#ifndef _D3D11CANVASWINDOWGRAPHICS_HPP
#define _D3D11CANVASWINDOWGRAPHICS_HPP

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <D3D11.h>

#include <memory>

#include "CanvasWindowGraphics.hpp"

namespace D3D11
{

class D3D11CanvasWindowGraphics;
typedef std::shared_ptr<D3D11CanvasWindowGraphics> D3D11CanvasWindowGraphicsPtr;

class D3D11CanvasWindowGraphics : public CanvasWindowGraphics
{

public:

	static D3D11CanvasWindowGraphicsPtr Create(HWND hWnd, ID3D11Device* pDevice, IDXGIFactory1* pDXGIFactory);
	~D3D11CanvasWindowGraphics();

	// CanvasWindowGraphics implementation
	virtual void OnWMPaint();

private:

	D3D11CanvasWindowGraphics();
	bool CreateInternal(HWND hWnd, ID3D11Device* pDevice, IDXGIFactory1* pDXGIFactory);

	void Present();

	HWND m_hWnd;
	IDXGISwapChain* m_pSwapChain;
	ID3D11RenderTargetView* m_pBackBufferRTV;

};

}

#endif
