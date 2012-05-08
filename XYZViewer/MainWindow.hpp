// MainWindow.hpp
// Nolan Check
// Created 5/7/2012

#ifndef _MAINWINDOW_HPP
#define _MAINWINDOW_HPP

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

class MainWindow
{

public:

	MainWindow();
	~MainWindow();

	void init(HINSTANCE hInstance, int nShowCmd);

private:

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWMCreate(HWND hWnd);
	LRESULT OnWMDestroy();

	HWND m_hWnd;

};

#endif
