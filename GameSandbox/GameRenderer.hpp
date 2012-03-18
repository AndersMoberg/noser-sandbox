// GameRenderer.hpp
// Nolan Check
// Created 3/17/2012

#ifndef _GAMERENDERER_HPP
#define _GAMERENDERER_HPP

#include <memory>

class GameRenderer;
typedef std::shared_ptr<GameRenderer> GameRendererPtr;

class GameRenderer
{

public:

	static GameRendererPtr Create();

private:

	GameRenderer();

};

#endif
