// D3D11DrawToolRenderer.cpp
// Nolan Check
// Created 1/5/2012

#include "D3D11DrawToolRenderer.hpp"

namespace D3D11
{

D3D11DrawToolRenderer::D3D11DrawToolRenderer()
{ }

D3D11DrawToolRendererPtr D3D11DrawToolRenderer::Create(CanvasImagePtr image)
{
	D3D11DrawToolRendererPtr p(new D3D11DrawToolRenderer);

	p->m_image = image;

	return p;
}

void D3D11DrawToolRenderer::RenderCircularGradient(const RectF& rc)
{
}

}
