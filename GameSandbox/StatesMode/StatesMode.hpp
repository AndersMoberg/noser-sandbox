// StatesMode.hpp
// Nolan Check
// Created 7/16/2012

#ifndef _STATESMODE_HPP
#define _STATESMODE_HPP

#include <memory>

#include "Game.hpp"
#include "D2DRenderer.hpp"

namespace StatesMode
{

class State
{
public:
	typedef std::shared_ptr<State> Ptr;
	virtual Ptr getNextState(const GameInput& input) = 0;
	virtual void render(D2DRenderer::Ptr renderer) = 0;
};

class StatesMode : public GameMode
{

public:

	typedef std::shared_ptr<StatesMode> Ptr;

	static Ptr create(Game::Ptr game);

	void Tick(const GameInput& input);
	void Render();

private:

	StatesMode();

	Game::WeakPtr m_game;

	D2DRenderer::Ptr m_renderer;

	State::Ptr m_curState;

};

}

#endif
