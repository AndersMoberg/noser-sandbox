// D2DRenderer.cpp
// Nolan Check
// Created 3/28/2012

#include "D2DRenderer.hpp"

D2DRenderer::D2DRenderer()
{ }

D2DRenderer::Ptr D2DRenderer::create(HWND hWnd)
{
	Ptr p(new D2DRenderer);

	CHECK_HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
		p->m_d2dFactory.Receive()));

	D2D1_RENDER_TARGET_PROPERTIES rtp = D2D1::RenderTargetProperties();
	rtp.dpiX = 96.0f;
	rtp.dpiY = 96.0f;

	RECT rc;
	GetClientRect(hWnd, &rc);
	D2D1_HWND_RENDER_TARGET_PROPERTIES hwrtp = D2D1::HwndRenderTargetProperties(
		hWnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top));

	CHECK_HR(p->m_d2dFactory->CreateHwndRenderTarget(&rtp, &hwrtp, p->m_d2dTarget.Receive()));

	return p;
}
