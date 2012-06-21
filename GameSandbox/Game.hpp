// Game.hpp
// Nolan Check
// Created 3/17/2012

#ifndef _GAME_HPP
#define _GAME_HPP

#include <memory>

#include "Camera.hpp"

struct GameInput
{
	Vector2f move;
	bool enter;
	bool esc;
};

class GameMode
{
public:
	typedef std::shared_ptr<GameMode> Ptr;
	virtual ~GameMode() { }
	virtual void Tick(const GameInput& input) = 0;
	virtual void Render() = 0;
};

class Game;

class GameModeSwitcher
{
public:
	typedef std::shared_ptr<GameModeSwitcher> Ptr;
	virtual ~GameModeSwitcher() { }
	virtual GameMode::Ptr createMode(Game* game) = 0;
};

class Game
{

public:

	static const unsigned int TICKS_PER_SEC;
	
	void init(HWND hWnd);

	void Tick(const GameInput& input);
	void Resize();
	void Render();

	void SwitchMode(GameModeSwitcher::Ptr nextMode);

	HWND GetHWnd() { return m_hWnd; }

private:

	HWND m_hWnd;

	GameMode::Ptr m_mode;

	GameModeSwitcher::Ptr m_nextMode;

};

#endif
