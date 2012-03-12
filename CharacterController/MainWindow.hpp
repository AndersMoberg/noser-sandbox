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

#include "D2DManager.hpp"
#include "Game.hpp"
#include "GLES2Manager.hpp"

class MainWindow;
typedef std::shared_ptr<MainWindow> MainWindowPtr;

class MainWindow
{

public:

	~MainWindow();
	static MainWindowPtr Create(HINSTANCE hInstance, int nShowCmd, GamePtr game);

	void Update();
	void Render();

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
	LRESULT OnWMKeyDown(WPARAM wParam);
	LRESULT OnWMKeyUp(WPARAM wParam);

	void CreateDeviceResources();
	void DestroyDeviceResources();

	bool m_exceptionThrown;
	std::exception m_exceptionProxy;

	HWND m_hWnd;

	GLES2ManagerPtr m_gles2Manager;
	D2DManagerPtr m_d2dManager;

	GamePtr m_game;

	// User input processing
	int m_leftToRightKeys; // -1: left; 0: none; 1: right
	int m_downToUpKeys; // -1: down; 0: none; 1: up
	bool m_spaceTrigger;

};

#endif
