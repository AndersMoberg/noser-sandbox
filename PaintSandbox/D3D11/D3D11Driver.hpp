// D3D11Driver.hpp
// Nolan Check
// Created 12/26/2011

#ifndef _D3D11DRIVER_HPP
#define _D3D11DRIVER_HPP

#include <d3d11.h>

#include <memory>

#include "BlendState.hpp"
#include "CircularGradientShader.hpp"
#include "Driver.hpp"
#include "D3D11CanvasWindowGraphics.hpp"
#include "D3D11Utils.hpp"
#include "Geometry.hpp"

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
	virtual CanvasWindowGraphicsPtr CreateWindowGraphics(HWND hWnd);
	virtual DriverImagePtr CreateImage(int width, int height);
	virtual DrawToolRendererPtr CreateDrawToolRenderer(CanvasImagePtr image);

	void RenderQuad(const RectF& rc);
	void RenderQuadToCanvas(CanvasImagePtr canvas, const RectF& rc);

	ID3D11Device* GetD3D11Device() { return m_pD3D11Device; }
	ID3D11DeviceContext* GetD3D11Context() { return m_pD3D11Context; }
	IDXGIFactory1* GetDXGIFactory() { return m_pDXGIFactory; }

	BlendStatePtr GetAlphaBlend() { return m_alphaBlend; }
	BlendStatePtr GetAlphaAccumBlend() { return m_alphaAccumBlend; }
	ID3D11SamplerState* GetBilinearSampler() { return m_pBilinearSampler; }
	PixelShaderPtr GetTexturedPixelShader() { return m_texturedPShader; }
	CircularGradientShaderPtr GetCircularGradientShader() { return m_circularGradientShader; }

private:

	D3D11Driver();

	ID3D11Device* m_pD3D11Device;
	ID3D11DeviceContext* m_pD3D11Context;
	IDXGIFactory1* m_pDXGIFactory;

	BlendStatePtr m_alphaBlend;
	BlendStatePtr m_alphaAccumBlend;

	ID3D11SamplerState* m_pBilinearSampler;

	ID3D11Buffer* m_pSimple2DQuad;

	VertexShaderPtr m_simple2DQuadVShader;
	ID3D11InputLayout* m_pSimple2DInputLayout;
	ConstantBufferPtr m_simple2DQuadParams;

	PixelShaderPtr m_texturedPShader;
	CircularGradientShaderPtr m_circularGradientShader;

};

}

#endif
