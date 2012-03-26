// GameRenderer.hpp
// Nolan Check
// Created 3/17/2012

#ifndef _GAMERENDERER_HPP
#define _GAMERENDERER_HPP

#include <memory>

#include <DWrite.h>

#include "GLES2Manager.hpp"
#include "WindowsUtils.hpp"

class GameRenderer
{

public:

	static GameRenderer* Create(HWND hWnd);

	GLES2Manager* GetGLES2Manager() { return m_gles2Manager.get(); }

	ComPtr<ID2D1Factory> GetD2DFactory() { return m_d2dFactory; }
	ComPtr<IDWriteFactory> GetDWriteFactory() { return m_dwriteFactory; }

	ComPtr<IDWriteTextFormat> GetDefaultTextFormat() { return m_defaultTextFormat; }

private:

	GameRenderer();

	std::unique_ptr<GLES2Manager> m_gles2Manager;

	ComPtr<ID2D1Factory> m_d2dFactory;
	ComPtr<IDWriteFactory> m_dwriteFactory;

	ComPtr<IDWriteTextFormat> m_defaultTextFormat;

};

#endif
