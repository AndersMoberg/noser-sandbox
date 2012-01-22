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

#include "Camera.hpp"
#include "World.hpp"

class MainWindow;
typedef std::shared_ptr<MainWindow> MainWindowPtr;

class MainWindow
{

public:

	~MainWindow();
	static MainWindowPtr Create(HINSTANCE hInstance, int nShowCmd);

private:

	MainWindow();

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWMCreate(HWND hwnd);
	LRESULT OnWMDestroy();
	LRESULT OnWMSize(LPARAM lParam);
	LRESULT OnWMPaint();

	bool CreateDeviceResources();
	void DestroyDeviceResources();

	void Render();

	HWND m_hWnd;

	ID2D1Factory* m_pD2DFactory;
	ID2D1HwndRenderTarget* m_pD2DTarget;

	CameraPtr m_camera;
	WorldPtr m_world;

};

#endif
