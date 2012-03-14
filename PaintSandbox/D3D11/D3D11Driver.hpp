// D3D11Driver.hpp
// Nolan Check
// Created 12/26/2011

#ifndef _D3D11DRIVER_HPP
#define _D3D11DRIVER_HPP

#include <d3d11.h>
#include <DWrite.h>

#include <memory>

#include "BlendState.hpp"
#include "Buffer.hpp"
#include "CircularGradientShader.hpp"
#include "Driver.hpp"
#include "D3D11CanvasWindowGraphics.hpp"
#include "D3D11Utils.hpp"
#include "Geometry.hpp"
#include "SamplerState.hpp"

namespace D3D11
{

class D3D11Driver;
typedef std::shared_ptr<D3D11Driver> D3D11DriverPtr;

class D3D11Driver : public Driver,
	public std::enable_shared_from_this<D3D11Driver>
{

public:

	static D3D11DriverPtr Create();
	~D3D11Driver();

	// Driver implementation
	virtual CanvasWindowGraphicsPtr CreateWindowGraphics(HWND hWnd, CameraPtr camera);
	virtual DriverImagePtr CreateImage(int width, int height);
	virtual DrawToolRendererPtr CreateDrawToolRenderer(CanvasImagePtr image);

	void RenderQuad(const Matrix3x2f& mat, const RectF& rc);
	void RenderQuadToCanvas(CanvasImagePtr canvas, const RectF& rc);
	void RenderD2DTarget(D2DTargetPtr d2dTarget);

	ComPtr<ID3D11Device> GetD3D11Device() { return m_d3d11Device; }
	ComPtr<ID3D11DeviceContext> GetD3D11Context() { return m_d3d11Context; }
	ComPtr<IDXGIFactory1> GetDXGIFactory() { return m_dxgiFactory; }
	ComPtr<ID3D10Device1> GetD3D10Device() { return m_d3d10Device; }
	ComPtr<ID2D1Factory> GetD2DFactory() { return m_d2dFactory; }
	ComPtr<IDWriteFactory> GetDWriteFactory() { return m_dwriteFactory; }

	BlendStatePtr GetOverBlend() { return m_overBlend; }

	SamplerStatePtr GetBilinearSampler() { return m_bilinearSampler; }

	PixelShaderPtr GetTexturedPixelShader() { return m_texturedPShader; }
	CircularGradientShaderPtr GetCircularGradientShader() { return m_circularGradientShader; }

private:

	D3D11Driver();

	ComPtr<ID3D11Device> m_d3d11Device;
	ComPtr<ID3D11DeviceContext> m_d3d11Context;
	ComPtr<IDXGIFactory1> m_dxgiFactory;
	ComPtr<ID3D10Device1> m_d3d10Device;

	ComPtr<ID2D1Factory> m_d2dFactory;
	ComPtr<IDWriteFactory> m_dwriteFactory;

	BlendStatePtr m_overBlend; // Porter-Duff "over" operation

	SamplerStatePtr m_bilinearSampler;

	BufferPtr m_simple2DQuad;

	VertexShaderPtr m_simple2DQuadVShader;
	InputLayoutPtr m_simple2DInputLayout;
	ConstantBufferPtr m_simple2DQuadParams;

	PixelShaderPtr m_texturedPShader;
	CircularGradientShaderPtr m_circularGradientShader;

};

}

#endif
