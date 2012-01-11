// ExtensibleImage.hpp
// Nolan Check
// Created 1/9/2012

#ifndef _EXTENSIBLEIMAGE_HPP
#define _EXTENSIBLEIMAGE_HPP

#include <map>
#include <memory>

#include "Geometry.hpp"

class ExtensibleImage;
typedef std::shared_ptr<ExtensibleImage> ExtensibleImagePtr;
class ExtensibleImageTile;
typedef std::shared_ptr<ExtensibleImageTile> ExtensibleImageTilePtr;

class ExtensibleImageTile
{

public:

	static ExtensibleImageTilePtr Create(const RectF& canvasRect);

private:

	ExtensibleImageTile();

	RectF m_canvasRect; // Rectangle occupied by this tile

};

// ExtensibleImage: A 2D image that can be extended by adding tiles
class ExtensibleImage
{

public:

	static ExtensibleImagePtr Create(const Vector2f& pixelsPerCanvas);

	// canvasRect is in canvas coordinates with a lower-left origin.
	// The image is stored in pixels which come in pixel coordinates with an
	// upper-left origin.
	void Extend(const RectF& canvasRect);

private:

	ExtensibleImage();

	Vector2f m_pixelsPerCanvas; // Number of pixels per canvas unit in x and y directions
	Matrix3x2f m_canvasToPixel; // Matrix to transform canvas coordinates to image pixel coordinates

	// Tile map. TODO: Use some kind of tree structure.
	// Tiles are addressed by tile coordinates with an upper-left origin.
	// Image pixels CAN be at negative coordinates!
	// TODO: This desperately needs better documentation.
	typedef std::map<Vector2i, ExtensibleImageTilePtr> TileMap;
	TileMap m_tiles;

};

#endif
