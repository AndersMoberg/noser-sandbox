// D2DLayer.hpp
// Nolan Check
// Created 3/12/2012

#ifndef _D2DLAYER_HPP
#define _D2DLAYER_HPP

#include <d2d1.h>

#include <GLES2/gl2.h>

#include <memory>
#include <vector>

#include "GLES2Texture.hpp"
#include "WindowsUtils.hpp"

class D2DLayer;
typedef std::shared_ptr<D2DLayer> D2DLayerPtr;

class D2DLayer
{

public:

	static D2DLayerPtr Create(HWND hWnd);

	void CreateDeviceResources();
	void DestroyDeviceResources();
	void Resize(D2D1_SIZE_U size);

	GLES2TexturePtr GetGLTexture();

	ComPtr<ID2D1RenderTarget> GetD2DTarget() { return m_d2dTarget; }

private:

	D2DLayer();

	HWND m_hWnd;
	ComPtr<ID2D1Factory> m_d2dFactory;
	ComPtr<ID2D1RenderTarget> m_d2dTarget;
	ComPtr<IWICBitmap> m_wicBitmap;
	std::vector<BYTE> m_imageBuffer;
	GLES2TexturePtr m_glTexture;

};

#endif
