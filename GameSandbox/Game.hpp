// Game.hpp
// Nolan Check
// Created 3/17/2012

#ifndef _GAME_HPP
#define _GAME_HPP

#include <memory>

#include "Camera.hpp"
#include "GameRenderer.hpp"
#include "RevealingText.hpp"

class Game;
typedef std::shared_ptr<Game> GamePtr;

class GameObject
{
public:
	virtual ~GameObject() { }
	virtual void Tick() = 0;
	virtual void Render() = 0;
};

class GameMode
{
public:
	virtual ~GameMode() { }
	virtual void Tick(const Vector2f& move) = 0;
	virtual void Render() = 0;
};

class Game
{

public:

	static const unsigned int TICKS_PER_SEC;

	static GamePtr Create(GameRenderer* renderer);

	void Tick(const Vector2f& move);
	void Render();

private:

	Game();

	GameRenderer* m_renderer;

	std::unique_ptr<GameMode> m_mode;

};

#endif
