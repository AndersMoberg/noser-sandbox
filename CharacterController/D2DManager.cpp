// D2DManager.cpp
// Nolan Check
// Created 3/12/2012

#include "D2DManager.hpp"

#include "WindowsUtils.hpp"

D2DManager::D2DManager()
	: m_d2dFactory(NULL),
	m_d2dTarget(NULL)
{ }

D2DManager::~D2DManager()
{
	SafeRelease(m_d2dTarget);
	SafeRelease(m_d2dFactory);
}

D2DManagerPtr D2DManager::Create(HWND hWnd)
{
	D2DManagerPtr p(new D2DManager);
	
	p->m_hWnd = hWnd;
	CHECK_HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &p->m_d2dFactory));

	p->CreateDeviceResources();

	return p;
}

void D2DManager::CreateDeviceResources()
{
	if (!m_d2dTarget)
	{
		RECT rc;
		GetClientRect(m_hWnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

		D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
		// Override DPI because we don't want DPI scaling on our graphics.
		rtProps.dpiX = 96.0f;
		rtProps.dpiY = 96.0f;

		CHECK_HR(m_d2dFactory->CreateHwndRenderTarget(
			rtProps, D2D1::HwndRenderTargetProperties(m_hWnd, size),
			&m_d2dTarget));
	
	}
}

void D2DManager::DestroyDeviceResources()
{
	SafeRelease(m_d2dTarget);
}

void D2DManager::Resize(D2D1_SIZE_U size)
{
	if (m_d2dTarget) {
		CHECK_HR(m_d2dTarget->Resize(size));
	}
}
