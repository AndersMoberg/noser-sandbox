// CanvasImage.cpp
// Nolan Check
// Created 12/27/2011

#include "CanvasImage.hpp"

#include "Driver.hpp"

CanvasImagePtr CanvasImage::Create(DriverPtr driver, const RectF& canvasRc,
	int width, int height)
{
	CanvasImagePtr result(new CanvasImage);
	if (!result->CreateInternal(driver, canvasRc, width, height)) {
		return NULL;
	}
	return result;
}

CanvasImage::CanvasImage()
{ }

bool CanvasImage::CreateInternal(DriverPtr driver, const RectF& canvasRc,
	int width, int height)
{
	m_width = width;
	m_height = height;

	m_driverImage = driver->CreateImage(width, height);
	if (!m_driverImage) {
		return false;
	}

	m_canvasRc = canvasRc;

	m_canvasToClip = Matrix3x2f::RectLerp(m_canvasRc,
		RectF(-1.0f, 1.0f, 1.0f, -1.0f));

	return true;
}
