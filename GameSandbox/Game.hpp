// Game.hpp
// Nolan Check
// Created 3/17/2012

#ifndef _GAME_HPP
#define _GAME_HPP

#include <memory>

#include "Camera.hpp"
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
	bool esc;
};

class GameMode
{
public:
	virtual ~GameMode() { }
	virtual void Tick(const GameInput& input) = 0;
	virtual void Render() = 0;
	virtual void Present() = 0;
};

class Game;

class GameModeSwitcher
{
public:
	virtual GameMode* CreateMode(Game* game) = 0;
};

class Game
{

public:

	static const unsigned int TICKS_PER_SEC;

	static Game* Create(HWND hWnd);

	void Tick(const GameInput& input);
	void Resize();
	void Render();
	void Present();

	void SwitchMode(GameModeSwitcher* nextMode);

	HWND GetHWnd() { return m_hWnd; }

private:

	Game();

	HWND m_hWnd;

	std::unique_ptr<GameMode> m_mode;

	std::unique_ptr<GameModeSwitcher> m_nextMode;

};

#endif
