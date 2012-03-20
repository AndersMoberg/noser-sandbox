// D2DLayer.cpp
// Nolan Check
// Created 3/19/2012

#include "D2DLayer.hpp"

#include <D2D1.h>
#include <wincodec.h>

#include <GLES2/gl2ext.h>

D2DLayer::D2DLayer()
{ }

D2DLayerPtr D2DLayer::Create(GameRendererPtr renderer)
{
	D2DLayerPtr p(new D2DLayer);

	p->m_renderer = renderer;

	p->m_glTexture = GLES2Texture::Create();

	p->CreateTargetResources();

	return p;
}

void D2DLayer::CreateTargetResources()
{
	if (!m_d2dTarget)
	{
		D2D1_SIZE_U size = D2D1::SizeU(m_renderer->GetGLES2Manager()->GetWidth(),
			m_renderer->GetGLES2Manager()->GetHeight());

		m_imageBuffer.resize(4*size.width*size.height);

		ComPtr<IWICImagingFactory> wicFactory;
		CHECK_HR(CoCreateInstance(CLSID_WICImagingFactory, NULL,
			CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)wicFactory.Receive()));

		CHECK_HR(wicFactory->CreateBitmapFromMemory(
			size.width, size.height, GUID_WICPixelFormat32bppPBGRA,
			4*size.width, 4*size.width*size.height, &m_imageBuffer[0],
			m_wicBitmap.Receive()));

		D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
		// Override DPI because we don't want DPI scaling on our graphics.
		rtProps.dpiX = 96.0f;
		rtProps.dpiY = 96.0f;

		CHECK_HR(m_renderer->GetD2DFactory()->CreateWicBitmapRenderTarget(
			m_wicBitmap, rtProps, m_d2dTarget.Receive()));
	}
}

void D2DLayer::DestroyTargetResources()
{
	m_d2dTarget.Release();
	m_wicBitmap.Release();
}

GLES2TexturePtr D2DLayer::GetGLTexture()
{
	ComPtr<IWICBitmapLock> lock;
	CHECK_HR(m_wicBitmap->Lock(NULL, WICBitmapLockRead, lock.Receive()));

	UINT dataSize;
	BYTE* data;
	CHECK_HR(lock->GetDataPointer(&dataSize, &data));

	D2D1_SIZE_U size = m_d2dTarget->GetPixelSize();

	glBindTexture(GL_TEXTURE_2D, m_glTexture->Get());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA_EXT, size.width, size.height, 0,
		GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);

	return m_glTexture;
}

ComPtr<ID2D1RenderTarget> D2DLayer::GetD2DTarget()
{
	CreateTargetResources();
	return m_d2dTarget;
}
