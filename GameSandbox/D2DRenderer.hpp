// D2DRenderer.hpp
// Nolan Check
// Created 3/28/2012

#ifndef	_D2DRENDERER_HPP
#define _D2DRENDERER_HPP

#include <memory>

#include <D2D1.h>

#include "WindowsUtils.hpp"

class D2DRenderer
{

public:

	typedef std::shared_ptr<D2DRenderer> Ptr;

	static Ptr create(HWND hWnd);

	ComPtr<ID2D1RenderTarget> GetD2DTarget() { return m_d2dTarget; }

private:

	D2DRenderer();

	ComPtr<ID2D1Factory> m_d2dFactory;
	ComPtr<ID2D1HwndRenderTarget> m_d2dTarget;

};

#endif
