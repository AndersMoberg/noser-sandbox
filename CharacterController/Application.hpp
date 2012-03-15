// Application.hpp
// Nolan Check
// Created 1/14/2012

#ifndef _APPLICATION_HPP
#define _APPLICATION_HPP

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <memory>

#include "MainWindow.hpp"

class Application;
typedef std::shared_ptr<Application> ApplicationPtr;

class Application
{

public:

	~Application();
	static ApplicationPtr Create(HINSTANCE hInstance, int nShowCmd);

	int MessageLoop();

private:

	Application();

	MainWindowPtr m_window;

};

#endif
