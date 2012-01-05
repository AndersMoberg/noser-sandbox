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

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	DriverImagePtr GetDriverImage() { return m_driverImage; }
	const RectF& GetCanvasRect() const { return m_canvasRc; }
	const Matrix3x2f& GetCanvasToClip() const { return m_canvasToClip; }

private:

	CanvasImage();

	int m_width;
	int m_height;
	DriverImagePtr m_driverImage;
	RectF m_canvasRc;

	Matrix3x2f m_canvasToClip;

};

#endif
