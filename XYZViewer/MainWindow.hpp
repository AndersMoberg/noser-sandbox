// MainWindow.hpp
// Nolan Check
// Created 5/7/2012

#ifndef _MAINWINDOW_HPP
#define _MAINWINDOW_HPP

#include <memory>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

class Application;

class MainWindow
{

public:

	~MainWindow();
	static std::unique_ptr<MainWindow> create(Application* app, HINSTANCE hInstance, int nShowCmd);

	HWND getHWnd();

private:
	
	MainWindow();

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT onWMCreate(HWND hWnd);
	LRESULT onWMDestroy();
	LRESULT onWMPaint();
	LRESULT onWMKeyDown(WPARAM wParam);

	Application* m_app;

	HWND m_hWnd;

};

#endif
