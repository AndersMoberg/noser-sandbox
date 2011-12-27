// CanvasWindow.hpp
// Nolan Check
// Created 12/26/2011

#ifndef _CANVASWINDOW_HPP
#define _CANVASWINDOW_HPP

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <memory>

class CanvasWindow;
typedef std::shared_ptr<CanvasWindow> CanvasWindowPtr;

class CanvasWindow
{

public:

	static CanvasWindowPtr Create(HINSTANCE hInstance, int nShowCmd);
	~CanvasWindow();

private:

	CanvasWindow();
	bool CreateInternal(HINSTANCE hInstance, int nShowCmd);

	void RegisterWindowClass(HINSTANCE hInstance);
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnWMCreate(HWND hwnd);
	LRESULT OnWMDestroy();

	HWND m_hWnd;

};

#endif
