// D2DRenderer.hpp
// Nolan Check
// Created 3/28/2012

#ifndef	_D2DRENDERER_HPP
#define _D2DRENDERER_HPP

#include <D2D1.h>

#include "WindowsUtils.hpp"

class D2DRenderer
{

public:

	void init(HWND hWnd);

	ComPtr<ID2D1RenderTarget> GetD2DTarget() { return m_d2dTarget; }

private:

	ComPtr<ID2D1Factory> m_d2dFactory;
	ComPtr<ID2D1HwndRenderTarget> m_d2dTarget;

};

#endif
