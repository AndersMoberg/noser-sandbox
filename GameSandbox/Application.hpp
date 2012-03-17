// Application.hpp
// Nolan Check
// Created 3/16/2012

#ifndef _APPLICATION_HPP
#define _APPLICATION_HPP

#include <memory>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include "MainWindow.hpp"

class Application;
typedef std::shared_ptr<Application> ApplicationPtr;

class Application
{

public:

	static ApplicationPtr Create(HINSTANCE hInstance, int nShowCmd);

	int MessagePump();

private:

	Application();

	MainWindowPtr m_window;

};

#endif
