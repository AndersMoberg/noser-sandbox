// Game.hpp
// Nolan Check
// Created 3/17/2012

#ifndef _GAME_HPP
#define _GAME_HPP

#include <memory>

#include "GameRenderer.hpp"

class Game;
typedef std::shared_ptr<Game> GamePtr;

class Game
{

public:

	static GamePtr Create(GameRendererPtr renderer);

	void Render();

private:

	Game();

	GameRendererPtr m_renderer;
	
	GLES2TexturePtr m_bgTexture;

};

#endif
