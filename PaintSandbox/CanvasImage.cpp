// CanvasImage.cpp
// Nolan Check
// Created 12/27/2011

#include "CanvasImage.hpp"

#include "Driver.hpp"

CanvasImage::CanvasImage()
{ }

CanvasImagePtr CanvasImage::Create(DriverPtr driver, const RectF& canvasRc,
	int width, int height)
{
	CanvasImagePtr p(new CanvasImage);

	p->m_width = width;
	p->m_height = height;

	p->m_driverImage = driver->CreateImage(width, height);
	if (!p->m_driverImage) {
		return NULL;
	}

	p->m_canvasRc = canvasRc;

	p->m_canvasToClip = Matrix3x2f::RectLerp(p->m_canvasRc,
		RectF(-1.0f, 1.0f, 1.0f, -1.0f));
	p->m_canvasToPixel = Matrix3x2f::RectLerp(p->m_canvasRc,
		RectF(0.0f, 0.0f, (float)width, (float)height));

	return p;
}
