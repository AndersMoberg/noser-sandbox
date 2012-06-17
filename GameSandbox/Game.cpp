// Game.cpp
// Nolan Check
// Created 3/17/2012

#include "Game.hpp"

#include <sstream>

#include "CharacterTestMode/CharacterTestMode.hpp"
#include "MainMenuMode/MainMenuMode.hpp"
#include "CharacterControllerMode/CharacterControllerMode.hpp"

const unsigned int Game::TICKS_PER_SEC = 120;

void Game::init(HWND hWnd)
{
	m_hWnd = hWnd;

	m_mode = MainMenuMode::MainMenuMode::create(this);
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
