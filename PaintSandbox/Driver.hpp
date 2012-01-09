// Driver.hpp
// Nolan Check
// Created 12/26/2011

#ifndef _DRIVER_HPP
#define _DRIVER_HPP

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <memory>

#include "CanvasWindowGraphics.hpp"

class Driver;
typedef std::shared_ptr<Driver> DriverPtr;
class DriverImage;
typedef std::shared_ptr<DriverImage> DriverImagePtr;

class Camera;
typedef std::shared_ptr<Camera> CameraPtr;

class Driver
{
public:
	virtual ~Driver() { }
	virtual CanvasWindowGraphicsPtr CreateWindowGraphics(HWND hWnd, CameraPtr camera) = 0;
	virtual DriverImagePtr CreateImage(int width, int height) = 0;
	virtual DrawToolRendererPtr CreateDrawToolRenderer(CanvasImagePtr image) = 0;

};

class DriverImage
{
public:
	virtual ~DriverImage() { }
};

#endif
