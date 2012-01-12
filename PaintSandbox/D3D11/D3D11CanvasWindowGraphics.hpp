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

#include "Camera.hpp"
#include "CanvasWindowGraphics.hpp"
#include "D2DTarget.hpp"

namespace D3D11
{

class D3D11CanvasWindowGraphics;
typedef std::shared_ptr<D3D11CanvasWindowGraphics> D3D11CanvasWindowGraphicsPtr;
class D3D11Driver;
typedef std::shared_ptr<D3D11Driver> D3D11DriverPtr;

class D3D11CanvasWindowGraphics : public CanvasWindowGraphics
{

public:

	static D3D11CanvasWindowGraphicsPtr Create(HWND hWnd, D3D11DriverPtr driver, CameraPtr camera);
	~D3D11CanvasWindowGraphics();

	// CanvasWindowGraphics implementation
	virtual void OnWMSize();
	virtual void OnWMPaint();
	virtual void SetCanvasImage(CanvasImagePtr image, ExtensibleImagePtr extensibleImage);

private:

	D3D11CanvasWindowGraphics();

	bool CreateSwapChainResources();
	void DestroySwapChainResources();

	void Render();
	void Present();

	void RenderPrintf(ID2D1RenderTarget* pD2DTarget, IDWriteTextFormat* textFormat,
		const D2D1_RECT_F& layoutRect, ID2D1Brush* defaultForegroundBrush,
		LPCWSTR msg, ...);

	HWND m_hWnd;
	D3D11DriverPtr m_driver;
	CameraPtr m_camera;
	
	IDWriteTextFormat* m_pTextFormat;

	IDXGISwapChain* m_pSwapChain;
	ID3D11RenderTargetView* m_pBackBufferRTV;
	D2DTargetPtr m_d2dTarget;

	CanvasImagePtr m_image;
	ExtensibleImagePtr m_extensibleImage;

};

}

#endif
