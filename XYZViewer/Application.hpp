// Application.hpp
// Nolan Check
// Created 5/7/2012

#ifndef _APPLICATION_HPP
#define _APPLICATION_HPP

#include "MainWindow.hpp"

class Application
{

public:

	void init(HINSTANCE hInstance, int nShowCmd);
	int messagePump();

private:

	MainWindow m_window;

};

#endif
