// CanvasImage.hpp
// Nolan Check
// Created 12/27/2011

#ifndef _CANVASIMAGE_HPP
#define _CANVASIMAGE_HPP

#include <memory>

#include "Geometry.hpp"

class CanvasImage;
typedef std::shared_ptr<CanvasImage> CanvasImagePtr;
class Driver;
typedef std::shared_ptr<Driver> DriverPtr;
class DriverImage;
typedef std::shared_ptr<DriverImage> DriverImagePtr;

class CanvasImage
{

public:

	static CanvasImagePtr Create(DriverPtr driver, const RectF& canvasRc, int width, int height);

	DriverImagePtr GetDriverImage() { return m_driverImage; }

private:

	CanvasImage();
	bool CreateInternal(DriverPtr driver, const RectF& canvasRc, int width, int height);

	DriverImagePtr m_driverImage;
	RectF m_canvasRc;

};

#endif
