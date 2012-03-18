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

	static const float TICKS_PER_SEC;

	static GamePtr Create(GameRendererPtr renderer);

	void Tick();
	void Render();

private:

	Game();

	GameRendererPtr m_renderer;

	unsigned long long m_curTick;
	
	GLES2TexturePtr m_bgTexture;

	Vector2f m_characterPos;
	Rectf m_characterRect;
	GLES2TexturePtr m_characterTexture;

};

#endif
