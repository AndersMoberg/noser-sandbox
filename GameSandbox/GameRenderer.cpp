// GameRenderer.cpp
// Nolan Check
// Created 3/17/2012

#include "GameRenderer.hpp"

GameRenderer::GameRenderer()
{ }

GameRendererPtr GameRenderer::Create()
{
	GameRendererPtr p(new GameRenderer);

	return p;
}
