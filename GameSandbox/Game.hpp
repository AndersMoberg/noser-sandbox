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

	static const unsigned long long TICKS_PER_SEC;

	static GamePtr Create(GameRendererPtr renderer);

	void Tick(const Vector2f& move);
	void Render();

private:

	Game();

	GameRendererPtr m_renderer;
	
	GLES2TexturePtr m_bgTexture;

	Vector2f m_characterPos;
	float m_characterSpeed;
	Rectf m_characterRect;
	GLES2TexturePtr m_characterTexture;

};

#endif
