// Game.cpp
// Nolan Check
// Created 3/17/2012

#include "Game.hpp"

#include <sstream>

#include "CharacterTestMode/CharacterTestMode.hpp"
#include "MainMenuMode/MainMenuMode.hpp"
#include "CharacterControllerMode/CharacterControllerMode.hpp"

const unsigned int Game::TICKS_PER_SEC = 3600;

Game::Game()
{ }

Game* Game::Create(GameRenderer* renderer)
{
	Game* p(new Game);

	p->m_renderer = renderer;

	//p->m_mode.reset(CharacterControllerMode::CharacterControllerMode::Create(renderer));
	//p->m_mode.reset(CharacterTestMode::CharacterTestMode::Create(renderer));
	p->m_mode.reset(MainMenuMode::MainMenuMode::Create(p, renderer));

	return p;
}

void Game::Tick(const GameInput& input)
{
	m_mode->Tick(input);

	if (m_nextMode)
	{
		m_mode.reset();
		m_mode.reset(m_nextMode->CreateMode(this, m_renderer));
		m_nextMode.reset();
	}
}

void Game::Render()
{
	m_mode->Render();
}

void Game::SwitchMode(GameModeSwitcher* nextMode)
{
	m_nextMode.reset(nextMode);
}
