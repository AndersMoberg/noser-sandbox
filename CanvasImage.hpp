// CanvasImage.hpp
// Nolan Check
// Created 12/27/2011

#ifndef _CANVASIMAGE_HPP
#define _CANVASIMAGE_HPP

#include <memory>

#include "Driver.hpp"

class CanvasImage;
typedef std::shared_ptr<CanvasImage> CanvasImagePtr;

class CanvasImage
{

public:

	static CanvasImagePtr Create(DriverPtr driver, int width, int height);

private:

	CanvasImage();
	bool CreateInternal(DriverPtr driver, int width, int height);

	DriverImagePtr m_driverImage;

};

#endif
