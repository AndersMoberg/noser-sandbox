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
	m_driverImage = driver->CreateImage(width, height);
	if (!m_driverImage) {
		return false;
	}

	m_canvasRc = canvasRc;

	return true;
}
