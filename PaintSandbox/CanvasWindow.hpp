// CanvasWindow.hpp
// Nolan Check
// Created 12/26/2011

#ifndef _CANVASWINDOW_HPP
#define _CANVASWINDOW_HPP

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <memory>

#include "Camera.hpp"
#include "CanvasImage.hpp"
#include "DrawTool.hpp"
#include "Driver.hpp"
#include "ExtensibleImage.hpp"

class CanvasWindow;
typedef std::shared_ptr<CanvasWindow> CanvasWindowPtr;

class CanvasWindow
{

public:

	static CanvasWindowPtr Create(DriverPtr driver, HINSTANCE hInstance, int nShowCmd);
	~CanvasWindow();

	bool IsExceptionThrown() const { return m_exceptionThrown; }
	const std::exception& GetExceptionProxy() const { return m_exceptionProxy; }

private:

	CanvasWindow();

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnWMCreate(HWND hwnd);
	LRESULT OnWMDestroy();
	LRESULT OnWMSize();
	LRESULT OnWMPaint();
	LRESULT OnWMMouseMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnWMLButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnWMLButtonUp(WPARAM wParam, LPARAM lParam);

	Vector2f TransformClientToCanvas(const Vector2f& v);

	bool m_exceptionThrown;
	std::exception m_exceptionProxy;

	DriverPtr m_driver;
	CameraPtr m_camera;
	CanvasImagePtr m_image;
	ExtensibleImagePtr m_extensibleImage; // TODO: Replace m_image with this
	DrawToolPtr m_drawTool;

	HWND m_hWnd;
	CanvasWindowGraphicsPtr m_graphics;

};

#endif
