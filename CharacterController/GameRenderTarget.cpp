// GameRenderTarget.cpp
// Nolan Check
// Created 2/20/2012

#include "GameRenderTarget.hpp"

#include "WindowsUtils.hpp"

GameRenderTarget::GameRenderTarget()
	: m_pDWriteFactory(NULL),
	m_pDialogTextFormat(NULL),
	m_pD2DTarget(NULL),
	m_pBlackBrush(NULL)
{ }

GameRenderTarget::~GameRenderTarget()
{
	ReleaseD2DTarget();
	SafeRelease(m_pDialogTextFormat);
	SafeRelease(m_pDWriteFactory);
}

GameRenderTargetPtr GameRenderTarget::Create()
{
	GameRenderTargetPtr p(new GameRenderTarget);
	
	CHECK_HR(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory), (IUnknown**)&p->m_pDWriteFactory));

	CHECK_HR(p->m_pDWriteFactory->CreateTextFormat(L"Calibri", NULL,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL, 24.0f, L"en-US", &p->m_pDialogTextFormat));

	return p;
}

void GameRenderTarget::SetD2DTarget(ID2D1RenderTarget* target)
{
	if (m_pD2DTarget != target)
	{
		m_pD2DTarget = target;

		// Create target-specific resources

		CHECK_HR(m_pD2DTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black), &m_pBlackBrush));
	}
}

void GameRenderTarget::ReleaseD2DTarget()
{
	// Release target-specific resources
	SafeRelease(m_pBlackBrush);
	m_pD2DTarget = NULL;
}
