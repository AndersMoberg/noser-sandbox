// Game.hpp
// Nolan Check
// Created 1/21/2012

#ifndef _GAME_HPP
#define _GAME_HPP

#include <memory>

#include "Camera.hpp"
#include "World.hpp"

class Game;
typedef std::shared_ptr<Game> GamePtr;

class Game
{

public:

	static GamePtr Create();

	void Render(ID2D1RenderTarget* target);

private:

	Game();

	CameraPtr m_camera;
	WorldPtr m_world;
	Vector2f m_characterPos;

};

#endif
