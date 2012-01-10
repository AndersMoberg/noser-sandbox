// ExtensibleImage.hpp
// Nolan Check
// Created 1/9/2012

#ifndef _EXTENSIBLEIMAGE_HPP
#define _EXTENSIBLEIMAGE_HPP

#include <memory>

class ExtensibleImage;
typedef std::shared_ptr<ExtensibleImage> ExtensibleImagePtr;

// ExtensibleImage: A 2D image that can be extended by adding tiles
class ExtensibleImage
{

public:

	static ExtensibleImagePtr Create();

	void Extend(const RectF& canvasRect);

private:

	ExtensibleImage();

	std::list<RectF> m_tiles;

};

#endif
