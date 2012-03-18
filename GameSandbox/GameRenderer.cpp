// GameRenderer.cpp
// Nolan Check
// Created 3/17/2012

#include "GameRenderer.hpp"

GameRenderer::GameRenderer()
{ }

GameRendererPtr GameRenderer::Create(HWND hWnd)
{
	GameRendererPtr p(new GameRenderer);

	p->m_gles2Manager = GLES2Manager::Create(hWnd);

	return p;
}
