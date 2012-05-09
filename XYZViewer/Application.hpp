// Application.hpp
// Nolan Check
// Created 5/7/2012

#ifndef _APPLICATION_HPP
#define _APPLICATION_HPP

#include "MainWindow.hpp"
#include "GLES2Renderer.hpp"

class Application
{

public:

	static std::unique_ptr<Application> create(HINSTANCE hInstance, int nShowCmd);

	int messagePump();

	void paint();

private:

	Application();

	std::unique_ptr<MainWindow> m_window;
	std::unique_ptr<GLES2Renderer> m_renderer;

};

#endif
