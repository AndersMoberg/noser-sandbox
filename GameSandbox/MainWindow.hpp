// MainWindow.hpp
// Nolan Check
// Created 3/16/2012

#ifndef _MAINWINDOW_HPP
#define _MAINWINDOW_HPP

#include <memory>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include "Game.hpp"
#include "GameRenderer.hpp"
#include "GLES2Manager.hpp"

class MainWindow;
typedef std::shared_ptr<MainWindow> MainWindowPtr;

class MainWindow
{

public:

	~MainWindow();
	static MainWindowPtr Create(HINSTANCE hInstance, int nShowCmd);

	bool IsExceptionThrown() const { return m_exceptionThrown; }
	const std::exception& GetExceptionProxy() const { return m_exceptionProxy; }

private:

	MainWindow();

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWMCreate(HWND hWnd);
	LRESULT OnWMDestroy();
	LRESULT OnWMSize();
	LRESULT OnWMPaint();

	void Update();
	void Render();
	void Present();

	HWND m_hWnd;

	bool m_exceptionThrown;
	std::exception m_exceptionProxy;

	long long m_curTime;
	long long m_frequency;

	GameRendererPtr m_renderer;
	GamePtr m_game;

};

#endif
