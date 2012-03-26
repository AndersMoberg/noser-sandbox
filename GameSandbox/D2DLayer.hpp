// D2DLayer.hpp
// Nolan Check
// Created 3/19/2012

#ifndef _D2DLAYER_HPP
#define _D2DLAYER_HPP

#include <memory>
#include <vector>

#include "GameRenderer.hpp"
#include "GLES2Texture.hpp"

class D2DLayer;
typedef std::shared_ptr<D2DLayer> D2DLayerPtr;

class D2DLayer
{

public:

	static D2DLayerPtr Create(GameRenderer* renderer);

	void DestroyTargetResources();

	void DrawOutlinedTextLayout(ComPtr<IDWriteTextLayout> textLayout,
		ComPtr<ID2D1Brush> fillBrush, ComPtr<ID2D1Brush> strokeBrush, float strokeWidth,
		const Vector2f& origin);

	GLES2Texture* GetGLTexture();
	ComPtr<ID2D1RenderTarget> GetD2DTarget();

private:

	void CreateTargetResources();

	D2DLayer();

	GameRenderer* m_renderer;

	std::unique_ptr<GLES2Texture> m_glTexture;

	std::vector<BYTE> m_imageBuffer;
	ComPtr<IWICBitmap> m_wicBitmap;
	ComPtr<ID2D1RenderTarget> m_d2dTarget;

};

#endif
