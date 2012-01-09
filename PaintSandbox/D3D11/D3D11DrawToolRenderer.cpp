// D3D11DrawToolRenderer.cpp
// Nolan Check
// Created 1/5/2012

#include "D3D11DrawToolRenderer.hpp"

#include <D3D11.h>

#include "D3D11Driver.hpp"
#include "D3D11Image.hpp"

namespace D3D11
{

D3D11DrawToolRenderer::D3D11DrawToolRenderer()
{ }

D3D11DrawToolRendererPtr D3D11DrawToolRenderer::Create(D3D11DriverPtr driver, CanvasImagePtr image)
{
	D3D11DrawToolRendererPtr p(new D3D11DrawToolRenderer);

	p->m_driver = driver;
	p->m_image = image;

	return p;
}

void D3D11DrawToolRenderer::RenderCircularGradient(const RectF& rc, float weight)
{
	ID3D11DeviceContext* pContext = m_driver->GetD3D11Context();
	D3D11::D3D11ImagePtr drvImage = std::static_pointer_cast<D3D11::D3D11Image, DriverImage>(
		m_image->GetDriverImage());

	// Set up rasterizer
	D3D11_VIEWPORT vp = CD3D11_VIEWPORT(0.0f, 0.0f, (FLOAT)m_image->GetWidth(), (FLOAT)m_image->GetHeight());
	pContext->RSSetViewports(1, &vp);

	// Set up output merger
	pContext->OMSetBlendState(m_driver->GetOverBlend()->Get(), NULL, 0xFFFFFFFF);

	// Set up pixel shader
	m_driver->GetCircularGradientShader()->Setup(pContext, weight);

	m_driver->RenderQuadToCanvas(m_image, rc);
}

}
