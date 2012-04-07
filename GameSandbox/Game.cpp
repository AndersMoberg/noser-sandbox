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

std::unique_ptr<Game> Game::create(HWND hWnd)
{
	std::unique_ptr<Game> p(new Game);

	p->m_hWnd = hWnd;

	p->m_mode = MainMenuMode::MainMenuMode::create(p.get());

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
