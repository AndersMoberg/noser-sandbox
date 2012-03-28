// D2DLayer.hpp
// Nolan Check
// Created 3/19/2012

#ifndef _D2DLAYER_HPP
#define _D2DLAYER_HPP

#include <memory>
#include <vector>

#include <D2D1.h>
#include <wincodec.h>

#include "GLES2Renderer.hpp"
#include "GLES2Texture.hpp"
#include "WindowsUtils.hpp"

class D2DLayer
{

public:

	D2DLayer();
	void Create(GLES2Renderer* renderer);

	void DestroyTargetResources();

	GLES2Texture* GetGLTexture();
	ComPtr<ID2D1RenderTarget> GetD2DTarget();

private:

	// Disallow copy and assign
	D2DLayer(const D2DLayer&);
	D2DLayer& operator=(const D2DLayer&);

	void CreateTargetResources();

	GLES2Renderer* m_renderer;

	ComPtr<ID2D1Factory> m_d2dFactory;

	std::unique_ptr<GLES2Texture> m_glTexture;

	std::vector<BYTE> m_imageBuffer;
	ComPtr<IWICBitmap> m_wicBitmap;
	ComPtr<ID2D1RenderTarget> m_d2dTarget;

};

#endif
