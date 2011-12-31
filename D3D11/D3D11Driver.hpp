// D3D11Driver.hpp
// Nolan Check
// Created 12/26/2011

#ifndef _D3D11DRIVER_HPP
#define _D3D11DRIVER_HPP

#include <d3d11.h>

#include <memory>

#include "Driver.hpp"
#include "D3D11CanvasWindowGraphics.hpp"
#include "D3D11Utils.hpp"

namespace D3D11
{

class D3D11Driver;
typedef std::shared_ptr<D3D11Driver> D3D11DriverPtr;

class D3D11Driver : public Driver
{

public:

	static D3D11DriverPtr Create();
	~D3D11Driver();

	// Driver implementation
	CanvasWindowGraphicsPtr CreateWindowGraphics(HWND hWnd);
	DriverImagePtr CreateImage(int width, int height);

private:

	D3D11Driver();
	bool CreateInternal();

	ID3D11Device* m_pD3D11Device;
	ID3D11DeviceContext* m_pD3D11Context;
	IDXGIFactory1* m_pDXGIFactory;

	ID3D11Buffer* m_pSimple2DQuad;

	VertexShaderPtr m_simple2DQuadVShader;

};

}

#endif
