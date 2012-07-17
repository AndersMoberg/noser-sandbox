// StatesMode.cpp
// Nolan Check
// Created 7/16/2012

#include "StatesMode.hpp"

#include "MainMenuMode/MainMenuMode.hpp"

namespace StatesMode
{

class BlueState;

class RedState : public State, public std::enable_shared_from_this<RedState>
{
public:
	typedef std::shared_ptr<RedState> Ptr;
	static Ptr create()
	{
		return Ptr(new RedState);
	}
	virtual State::Ptr getNextState(const GameInput& input);
	virtual void render(D2DRenderer::Ptr renderer)
	{
		ComPtr<ID2D1RenderTarget> d2dTarget = renderer->GetD2DTarget();
		d2dTarget->BeginDraw();
		d2dTarget->Clear(D2D1::ColorF(D2D1::ColorF::Red));
		d2dTarget->EndDraw();
	}
private:
	RedState() { }
};

class BlueState : public State, public std::enable_shared_from_this<BlueState>
{
public:
	typedef std::shared_ptr<BlueState> Ptr;
	static Ptr create()
	{
		return Ptr(new BlueState);
	}
	virtual State::Ptr getNextState(const GameInput& input)
	{
		if (input.enterTrigger) {
			return RedState::create();
		} else {
			return shared_from_this();
		}
	}
	virtual void render(D2DRenderer::Ptr renderer)
	{
		ComPtr<ID2D1RenderTarget> d2dTarget = renderer->GetD2DTarget();
		d2dTarget->BeginDraw();
		d2dTarget->Clear(D2D1::ColorF(D2D1::ColorF::Blue));
		d2dTarget->EndDraw();
	}
private:
	BlueState() { }
};

State::Ptr RedState::getNextState(const GameInput& input)
{
	if (input.enterTrigger) {
		return BlueState::create();
	} else {
		return shared_from_this();
	}
}

StatesMode::StatesMode()
{ }

StatesMode::Ptr StatesMode::create(Game::Ptr game)
{
	StatesMode::Ptr p(new StatesMode);

	p->m_game = game;
	p->m_renderer = D2DRenderer::create(game->GetHWnd());

	p->m_curState = RedState::create();

	return p;
}

GameMode::Ptr StatesMode::Tick(const GameInput& input)
{
	Game::Ptr game = m_game.lock();
	assert(game);

	m_curState = m_curState->getNextState(input);
	if (!m_curState) {
		return MainMenuMode::MainMenuMode::create(game);
	} else {
		return shared_from_this();
	}
}

void StatesMode::Render()
{
	m_curState->render(m_renderer);
}

}
