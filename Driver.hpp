// Driver.hpp
// Nolan Check
// Created 12/26/2011

#ifndef _DRIVER_HPP
#define _DRIVER_HPP

#include <memory>

#include "CanvasWindowGraphics.hpp"

class Driver;
typedef std::shared_ptr<Driver> DriverPtr;

class Driver
{

public:

	virtual CanvasWindowGraphicsPtr CreateWindowGraphics(HWND hWnd) = 0;

};

#endif
