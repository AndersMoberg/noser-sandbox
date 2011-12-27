// CanvasWindow.hpp
// Nolan Check
// Created 12/26/2011

#ifndef _CANVASWINDOW_HPP
#define _CANVASWINDOW_HPP

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <memory>

#include "Driver.hpp"

class CanvasWindow;
typedef std::shared_ptr<CanvasWindow> CanvasWindowPtr;

class CanvasWindow
{

public:

	static CanvasWindowPtr Create(DriverPtr driver, HINSTANCE hInstance, int nShowCmd);
	~CanvasWindow();

private:

	CanvasWindow();
	bool CreateInternal(DriverPtr driver, HINSTANCE hInstance, int nShowCmd);

	void RegisterWindowClass(HINSTANCE hInstance);
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnWMCreate(HWND hwnd);
	LRESULT OnWMDestroy();
	LRESULT OnWMPaint();

	DriverPtr m_driver;

	HWND m_hWnd;
	CanvasWindowGraphicsPtr m_graphics;

};

#endif
