// Game.hpp
// Nolan Check
// Created 3/17/2012

#ifndef _GAME_HPP
#define _GAME_HPP

#include <memory>

#include "Camera.hpp"
#include "GameRenderer.hpp"
#include "RevealingText.hpp"
#include "CharacterTestMode.hpp"

class Game;
typedef std::shared_ptr<Game> GamePtr;

class GameObject;
typedef std::shared_ptr<GameObject> GameObjectPtr;

class GameObject
{
public:
	virtual ~GameObject() { }
	virtual void Tick() = 0;
	virtual void Render() = 0;
};

class Game
{

public:

	static const unsigned int TICKS_PER_SEC;

	static GamePtr Create(GameRendererPtr renderer);

	void Tick(const Vector2f& move);
	void Render();

private:

	Game();

	GameRendererPtr m_renderer;

	CharacterTestModePtr m_mode;

};

#endif
