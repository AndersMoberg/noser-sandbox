// ExtensibleImage.cpp
// Nolan Check
// Created 1/10/2012

#include "ExtensibleImage.hpp"

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
