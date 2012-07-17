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
	bool enterTrigger;
	bool esc;
	bool escTrigger;
};

class GameMode
{
public:
	typedef std::shared_ptr<GameMode> Ptr;
	virtual ~GameMode() { }
	virtual Ptr Tick(const GameInput& input) = 0;
	virtual void Render() = 0;
};

class Game : public std::enable_shared_from_this<Game>
{

public:

	static const unsigned int TICKS_PER_SEC;

	typedef std::shared_ptr<Game> Ptr;
	typedef std::weak_ptr<Game> WeakPtr;
	
	static Ptr create(HWND hWnd);

	void Tick(const GameInput& input);
	void Resize();
	void Render();

	HWND GetHWnd() { return m_hWnd; }

private:

	Game();

	HWND m_hWnd;

	GameMode::Ptr m_mode;

};

#endif
