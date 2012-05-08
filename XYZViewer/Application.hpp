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

	void init(HINSTANCE hInstance, int nShowCmd);
	int messagePump();

private:

	MainWindow m_window;
	GLES2Renderer m_renderer;

};

#endif
