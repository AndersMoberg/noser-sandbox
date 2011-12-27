// CanvasImage.cpp
// Nolan Check
// Created 12/27/2011

#include "CanvasImage.hpp"

CanvasImagePtr CanvasImage::Create(DriverPtr driver, int width, int height)
{
	CanvasImagePtr result(new CanvasImage);
	if (!result->CreateInternal(driver, width, height)) {
		return NULL;
	}
	return result;
}

CanvasImage::CanvasImage()
{ }

bool CanvasImage::CreateInternal(DriverPtr driver, int width, int height)
{
	m_driverImage = driver->CreateImage(width, height);
	if (!m_driverImage) {
		return false;
	}

	return true;
}
