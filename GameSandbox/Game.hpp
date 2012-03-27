// Game.hpp
// Nolan Check
// Created 3/17/2012

#ifndef _GAME_HPP
#define _GAME_HPP

#include <memory>

#include "Camera.hpp"
#include "GameRenderer.hpp"
#include "RevealingText.hpp"

class GameObject
{
public:
	virtual ~GameObject() { }
	virtual void Tick() = 0;
	virtual void Render() = 0;
};

struct GameInput
{
	Vector2f move;
	bool enter;
};

class GameMode
{
public:
	virtual ~GameMode() { }
	virtual void Tick(const GameInput& input) = 0;
	virtual void Render() = 0;
};

class Game
{

public:

	static const unsigned int TICKS_PER_SEC;

	static Game* Create(GameRenderer* renderer);

	void Tick(const GameInput& input);
	void Render();

	void SetMode(GameMode* newMode);

private:

	Game();

	GameRenderer* m_renderer;

	std::unique_ptr<GameMode> m_mode;

};

#endif
