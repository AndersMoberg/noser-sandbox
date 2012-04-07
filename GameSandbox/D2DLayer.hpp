// D2DLayer.hpp
// Nolan Check
// Created 3/19/2012

#ifndef _D2DLAYER_HPP
#define _D2DLAYER_HPP

#include <memory>

#include <D2D1.h>
#include <wincodec.h>

#include "GLES2Renderer.hpp"
#include "GLES2Texture.hpp"
#include "WindowsUtils.hpp"

class D2DLayer
{

public:

	static std::unique_ptr<D2DLayer> create(GLES2Renderer* renderer);

	void DestroyTargetResources();

	GLES2Texture* GetGLTexture();
	ID2D1RenderTarget* getD2DTarget();

private:

	D2DLayer();

	void CreateTargetResources();

	GLES2Renderer* m_renderer;

	ComPtr<ID2D1Factory> m_d2dFactory;

	std::unique_ptr<GLES2Texture> m_glTexture;

	ComPtr<IWICBitmap> m_wicBitmap;
	ComPtr<ID2D1RenderTarget> m_d2dTarget;

};

#endif
