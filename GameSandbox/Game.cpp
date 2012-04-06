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

Game* Game::Create(HWND hWnd)
{
	Game* p(new Game);

	p->m_hWnd = hWnd;

	//p->m_mode.reset(CharacterControllerMode::CharacterControllerMode::Create(renderer));
	//p->m_mode.reset(CharacterTestMode::CharacterTestMode::Create(renderer));
	p->m_mode = MainMenuMode::MainMenuMode::create(p);

	return p;
}

void Game::Tick(const GameInput& input)
{
	m_mode->Tick(input);

	if (m_nextMode)
	{
		m_mode.reset();
		m_mode = m_nextMode->createMode(this);
		m_nextMode.reset();
	}
}

void Game::Resize()
{
	// TODO: Send resize message to mode
}

void Game::Render()
{
	m_mode->Render();
}

void Game::SwitchMode(GameModeSwitcher* nextMode)
{
	m_nextMode.reset(nextMode);
}
