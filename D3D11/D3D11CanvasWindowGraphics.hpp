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
class D3D11Driver;
typedef std::shared_ptr<D3D11Driver> D3D11DriverPtr;

class D3D11CanvasWindowGraphics : public CanvasWindowGraphics
{

public:

	static D3D11CanvasWindowGraphicsPtr Create(HWND hWnd, D3D11DriverPtr driver);
	~D3D11CanvasWindowGraphics();

	// CanvasWindowGraphics implementation
	virtual void OnWMPaint();
	virtual void SetCanvasImage(CanvasImagePtr image);

private:

	D3D11CanvasWindowGraphics();
	bool CreateInternal(HWND hWnd, D3D11DriverPtr driver);

	void Render();
	void Present();

	HWND m_hWnd;
	D3D11DriverPtr m_driver;

	IDXGISwapChain* m_pSwapChain;
	ID3D11RenderTargetView* m_pBackBufferRTV;

	CanvasImagePtr m_image;

};

}

#endif
