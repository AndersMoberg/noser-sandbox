// D2DLayer.cpp
// Nolan Check
// Created 3/19/2012

#include "D2DLayer.hpp"

#include <D2D1.h>
#include <wincodec.h>

#include <GLES2/gl2ext.h>

#include "OutlinedTextRenderer.hpp"

D2DLayer::D2DLayer()
{ }

void D2DLayer::Create(GLES2Renderer* renderer)
{
	m_renderer = renderer;

	CHECK_HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
		m_d2dFactory.Receive()));

	m_glTexture.reset(new GLES2Texture);

	CreateTargetResources();
}

void D2DLayer::CreateTargetResources()
{
	if (!m_d2dTarget)
	{
		D2D1_SIZE_U size = D2D1::SizeU(m_renderer->GetWidth(),
			m_renderer->GetHeight());

		ComPtr<IWICImagingFactory> wicFactory;
		CHECK_HR(CoCreateInstance(CLSID_WICImagingFactory, NULL,
			CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)wicFactory.Receive()));

		CHECK_HR(wicFactory->CreateBitmap(size.width, size.height,
			GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnDemand,
			m_wicBitmap.Receive()));

		D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
		// Override DPI because we don't want DPI scaling on our graphics.
		rtProps.dpiX = 96.0f;
		rtProps.dpiY = 96.0f;

		CHECK_HR(m_d2dFactory->CreateWicBitmapRenderTarget(
			m_wicBitmap, rtProps, m_d2dTarget.Receive()));
	}
}

void D2DLayer::DestroyTargetResources()
{
	m_d2dTarget.Release();
	m_wicBitmap.Release();
}

GLES2Texture* D2DLayer::GetGLTexture()
{
	ComPtr<IWICBitmapLock> lock;
	CHECK_HR(m_wicBitmap->Lock(NULL, WICBitmapLockRead, lock.Receive()));

	UINT dataSize;
	BYTE* data;
	CHECK_HR(lock->GetDataPointer(&dataSize, &data));

	D2D1_SIZE_U size = m_d2dTarget->GetPixelSize();

	glBindTexture(GL_TEXTURE_2D, m_glTexture->get());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA_EXT, size.width, size.height, 0,
		GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);

	return m_glTexture.get();
}

ID2D1RenderTarget* D2DLayer::getD2DTarget()
{
	CreateTargetResources();
	return m_d2dTarget;
}
