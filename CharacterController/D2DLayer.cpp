// D2DLayer.cpp
// Nolan Check
// Created 3/12/2012

#include "D2DLayer.hpp"

#include <wincodec.h>

#include <GLES2/gl2ext.h>

#include "WindowsUtils.hpp"

D2DLayer::D2DLayer()
{ }

D2DLayerPtr D2DLayer::Create(HWND hWnd)
{
	D2DLayerPtr p(new D2DLayer);

	p->m_glTexture = GLES2Texture::Create();
	
	p->m_hWnd = hWnd;
	CHECK_HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, p->m_d2dFactory.Receive()));

	p->CreateDeviceResources();

	return p;
}

void D2DLayer::CreateDeviceResources()
{
	if (!m_d2dTarget)
	{
		RECT rc;
		GetClientRect(m_hWnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

		m_imageBuffer.resize(4*size.width*size.height);

		IWICImagingFactory* wicFactory = NULL;
		CHECK_HR(CoCreateInstance(CLSID_WICImagingFactory, NULL,
			CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&wicFactory));

		CHECK_HR(wicFactory->CreateBitmapFromMemory(
			size.width, size.height, GUID_WICPixelFormat32bppPBGRA,
			4*size.width, 4*size.width*size.height, &m_imageBuffer[0],
			m_wicBitmap.Receive()));

		D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
		// Override DPI because we don't want DPI scaling on our graphics.
		rtProps.dpiX = 96.0f;
		rtProps.dpiY = 96.0f;

		CHECK_HR(m_d2dFactory->CreateWicBitmapRenderTarget(
			m_wicBitmap, rtProps, m_d2dTarget.Receive()));

		SafeRelease(wicFactory);
	}
}

void D2DLayer::DestroyDeviceResources()
{
	m_d2dTarget.Release();
	m_wicBitmap.Release();
}

void D2DLayer::Resize(D2D1_SIZE_U size)
{
	if (m_d2dTarget) {
		// FIXME: More than this must be recreated.
		DestroyDeviceResources();
		CreateDeviceResources();
	}
}

GLES2TexturePtr D2DLayer::GetGLTexture()
{
	IWICBitmapLock* lock = NULL;
	CHECK_HR(m_wicBitmap->Lock(NULL, WICBitmapLockRead, &lock));

	UINT dataSize;
	BYTE* data;
	CHECK_HR(lock->GetDataPointer(&dataSize, &data));

	D2D1_SIZE_U size = m_d2dTarget->GetPixelSize();

	glBindTexture(GL_TEXTURE_2D, m_glTexture->Get());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA_EXT, size.width, size.height, 0,
		GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);

	SafeRelease(lock);

	return m_glTexture;
}
