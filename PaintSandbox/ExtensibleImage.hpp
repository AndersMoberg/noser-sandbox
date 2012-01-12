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

	const RectF& GetCanvasRect() const { return m_canvasRect; }

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
	
	// Tile map. TODO: Use some kind of tree structure.
	// Tiles are addressed by tile coordinates with an upper-left origin.
	// Image pixels CAN be at negative coordinates!
	// TODO: This desperately needs better documentation.
	class Vector2iLess : std::binary_function<Vector2i, Vector2i, bool>
	{
	public:
		bool operator()(const Vector2i& lhs, const Vector2i& rhs) const {
			if (lhs.x < rhs.x) {
				return true;
			} else if (lhs.x > rhs.x) {
				return false;
			} else {
				return lhs.y < rhs.y;
			}
		}
	};
	typedef std::map<Vector2i, ExtensibleImageTilePtr, Vector2iLess> TileMap;

	TileMap& GetTileMap() { return m_tiles; }

private:

	ExtensibleImage();

	Vector2f m_pixelsPerCanvas; // Number of pixels per canvas unit in x and y directions
	Matrix3x2f m_canvasToPixel; // Matrix to transform canvas coordinates to image pixel coordinates

	TileMap m_tiles;

};

#endif
