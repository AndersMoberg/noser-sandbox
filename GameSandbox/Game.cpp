// Game.cpp
// Nolan Check
// Created 3/17/2012

#include "Game.hpp"

#include <sstream>

#include "CharacterTestMode/CharacterTestMode.hpp"
#include "MainMenuMode/MainMenuMode.hpp"
#include "CharacterControllerMode/CharacterControllerMode.hpp"

const unsigned int Game::TICKS_PER_SEC = 120;

Game::Game()
{ }

Game::Ptr Game::create(HWND hWnd)
{
	Ptr p(new Game);

	p->m_hWnd = hWnd;

	p->m_mode = MainMenuMode::MainMenuMode::create(p);

	return p;
}

void Game::Tick(const GameInput& input)
{
	m_mode->Tick(input);

	if (m_nextMode)
	{
		m_mode.reset();
		m_mode = m_nextMode->createMode(shared_from_this());
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

void Game::SwitchMode(GameModeSwitcher::Ptr nextMode)
{
	m_nextMode = nextMode;
}
