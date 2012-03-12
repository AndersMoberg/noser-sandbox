// D2DManager.hpp
// Nolan Check
// Created 3/12/2012

#ifndef _D2DMANAGER_HPP
#define _D2DMANAGER_HPP

#include <d2d1.h>

#include <memory>

class D2DManager;
typedef std::shared_ptr<D2DManager> D2DManagerPtr;

class D2DManager
{

public:

	~D2DManager();
	static D2DManagerPtr Create(HWND hWnd);

	void CreateDeviceResources();
	void DestroyDeviceResources();
	void Resize(D2D1_SIZE_U size);

	ID2D1RenderTarget* GetD2DTarget() { return m_d2dTarget; }

private:

	D2DManager();

	HWND m_hWnd;
	ID2D1Factory* m_d2dFactory;
	ID2D1HwndRenderTarget* m_d2dTarget;

};

#endif
