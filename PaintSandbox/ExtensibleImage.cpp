// ExtensibleImage.cpp
// Nolan Check
// Created 1/10/2012

#include "ExtensibleImage.hpp"

#include <cassert>

#include "WindowsUtils.hpp"

// TODO: Make this adjustable?
static const int TILE_SIZE = 256;

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

	RectI tileRect(
		(int)floor(pixelRectf.left / TILE_SIZE), (int)floor(pixelRectf.top / TILE_SIZE),
		(int)ceil(pixelRectf.right / TILE_SIZE), (int)ceil(pixelRectf.bottom / TILE_SIZE));
	assert(tileRect.left < tileRect.right);
	assert(tileRect.top < tileRect.bottom);
/*
	Debug("canvas rect %f, %f, %f, %f translates to pixel rect %d, %d, %d, %d\n",
		canvasRect.left, canvasRect.top, canvasRect.right, canvasRect.bottom,
		pixelRect.left, pixelRect.top, pixelRect.right, pixelRect.bottom);*/

	// Add all the necessary tiles that canvasRect covers
	for (int y = tileRect.top; y < tileRect.bottom; ++y)
	{
		for (int x = tileRect.left; x < tileRect.right; ++x)
		{
			AddTile(Vector2i(x, y));
		}
	}
}

void ExtensibleImage::AddTile(const Vector2i& address)
{
	// Don't replace a tile that already exists
	if (m_tiles.find(address) != m_tiles.end()) {
		return;
	}

	RectF canvasRect(
		address.x * TILE_SIZE / m_pixelsPerCanvas.x,
		-address.y * TILE_SIZE / m_pixelsPerCanvas.y,
		(address.x+1) * TILE_SIZE / m_pixelsPerCanvas.x,
		-(address.y+1) * TILE_SIZE / m_pixelsPerCanvas.y);

	m_tiles[address] = ExtensibleImageTile::Create(canvasRect);
}
