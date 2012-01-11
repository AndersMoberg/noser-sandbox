// ExtensibleImage.cpp
// Nolan Check
// Created 1/10/2012

#include "ExtensibleImage.hpp"

#include "WindowsUtils.hpp"

ExtensibleImageTile::ExtensibleImageTile()
{ }

ExtensibleImageTilePtr ExtensibleImageTile::Create(const RectF& canvasRect)
{
	ExtensibleImageTilePtr p(new ExtensibleImageTile);

	p->m_canvasRect = canvasRect;

	return p;
}

ExtensibleImage::ExtensibleImage()
{ }

ExtensibleImagePtr ExtensibleImage::Create(const Vector2f& pixelsPerCanvas)
{
	ExtensibleImagePtr p(new ExtensibleImage);

	p->m_pixelsPerCanvas = pixelsPerCanvas;
	p->m_canvasToPixel = Matrix3x2f::RectLerp(
		RectF(0.0f, 0.0f, 1.0f, -1.0f),
		RectF(0.0f, 0.0f, pixelsPerCanvas.x, pixelsPerCanvas.y));

	return p;
}

void ExtensibleImage::Extend(const RectF& canvasRect)
{
	// Convert canvasRect to pixel coordinates
	RectF pixelRectf(
		m_canvasToPixel.m11 * canvasRect.left + m_canvasToPixel.m31,
		m_canvasToPixel.m22 * canvasRect.top + m_canvasToPixel.m32,
		m_canvasToPixel.m11 * canvasRect.right + m_canvasToPixel.m31,
		m_canvasToPixel.m22 * canvasRect.bottom + m_canvasToPixel.m32);
	RectI pixelRect(
		(int)floor(pixelRectf.left), (int)floor(pixelRectf.top),
		(int)ceil(pixelRectf.right), (int)ceil(pixelRectf.bottom));

	Debug("canvas rect %f, %f, %f, %f translates to pixel rect %d, %d, %d, %d\n",
		canvasRect.left, canvasRect.top, canvasRect.right, canvasRect.bottom,
		pixelRect.left, pixelRect.top, pixelRect.right, pixelRect.bottom);
}
