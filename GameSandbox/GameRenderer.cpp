// GameRenderer.cpp
// Nolan Check
// Created 3/17/2012

#include "GameRenderer.hpp"

GameRenderer::GameRenderer()
{ }

GameRenderer* GameRenderer::Create(HWND hWnd)
{
	GameRenderer* p(new GameRenderer);

	p->m_gles2Manager.reset(GLES2Manager::Create(hWnd));

	CHECK_HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, p->m_d2dFactory.Receive()));

	CHECK_HR(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory), (IUnknown**)p->m_dwriteFactory.Receive()));

	CHECK_HR(p->m_dwriteFactory->CreateTextFormat(L"Kootenay", NULL,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL, 32.0f, L"en-US", p->m_defaultTextFormat.Receive()));

	return p;
}
