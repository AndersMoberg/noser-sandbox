// GameRenderer.hpp
// Nolan Check
// Created 3/17/2012

#ifndef _GAMERENDERER_HPP
#define _GAMERENDERER_HPP

#include <memory>

#include "GLES2Manager.hpp"

class GameRenderer;
typedef std::shared_ptr<GameRenderer> GameRendererPtr;

class GameRenderer
{

public:

	static GameRendererPtr Create(HWND hWnd);

	GLES2ManagerPtr GetGLES2Manager() { return m_gles2Manager; }

private:

	GameRenderer();

	GLES2ManagerPtr m_gles2Manager;

};

#endif
