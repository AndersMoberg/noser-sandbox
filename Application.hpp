// Application.hpp
// Nolan Check
// Created 12/26/2011

#ifndef _APPLICATION_HPP
#define _APPLICATION_HPP

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <memory>

#include "CanvasWindow.hpp"
#include "D3D11/D3D11Driver.hpp"

class Application;
typedef std::shared_ptr<Application> ApplicationPtr;

class Application
{

public:

	static ApplicationPtr Create(HINSTANCE hInstance, int nShowCmd);

	int MessageLoop();

private:

	Application();
	bool CreateInternal(HINSTANCE hInstance, int nShowCmd);

	D3D11::D3D11DriverPtr m_driver;
	CanvasWindowPtr m_canvasWindow;

};

#endif
