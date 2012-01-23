// MainWindow.hpp
// Nolan Check
// Created 1/14/2012

#ifndef _MAINWINDOW_HPP
#define _MAINWINDOW_HPP

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <d2d1.h>

#include <memory>

#include "Game.hpp"

class MainWindow;
typedef std::shared_ptr<MainWindow> MainWindowPtr;

class MainWindow
{

public:

	~MainWindow();
	static MainWindowPtr Create(HINSTANCE hInstance, int nShowCmd, GamePtr game);

	// C++ exceptions thrown in a window procedure get ignored. The exception
	// proxy is meant to pass them through.
	bool ExceptionThrown() const { return m_exceptionThrown; }
	const std::exception& GetExceptionProxy() const { return m_exceptionProxy; }

private:

	MainWindow();

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWMCreate(HWND hwnd);
	LRESULT OnWMDestroy();
	LRESULT OnWMSize(LPARAM lParam);
	LRESULT OnWMPaint();

	void CreateDeviceResources();
	void DestroyDeviceResources();

	void Update();
	void Render();

	bool m_exceptionThrown;
	std::exception m_exceptionProxy;

	HWND m_hWnd;

	ID2D1Factory* m_pD2DFactory;
	ID2D1HwndRenderTarget* m_pD2DTarget;

	GamePtr m_game;

};

#endif
