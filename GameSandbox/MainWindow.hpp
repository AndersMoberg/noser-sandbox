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
#include "GLES2Renderer.hpp"

class MainWindow
{

public:
	
	MainWindow();
	~MainWindow();
	void init(HINSTANCE hInstance, int nShowCmd);

	bool IsExceptionThrown() const { return m_exceptionThrown; }
	const std::exception& GetExceptionProxy() const { return m_exceptionProxy; }

	void process();

private:

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWMCreate(HWND hWnd);
	LRESULT OnWMDestroy();
	LRESULT OnWMSize();
	LRESULT OnWMPaint();
	LRESULT OnWMKeyDown(WPARAM wParam);
	LRESULT OnWMKeyUp(WPARAM wParam);

	void Update();
	void Render();

	HWND m_hWnd;

	bool m_exceptionThrown;
	std::exception m_exceptionProxy;

	long long m_curTime;
	long long m_frequency;

	Game::Ptr m_game;

	// User input processing
	int m_leftToRightKeys; // -1: left; 0: none; 1: right
	int m_downToUpKeys; // -1: down; 0: none; 1: up

};

#endif
