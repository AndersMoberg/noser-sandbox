// Game.cpp
// Nolan Check
// Created 3/17/2012

#include "Game.hpp"

#include <sstream>

const unsigned int Game::TICKS_PER_SEC = 3600;

Game::Game()
{ }

GamePtr Game::Create(GameRendererPtr renderer)
{
	GamePtr p(new Game);

	p->m_renderer = renderer;

	p->m_mode = CharacterTestMode::Create(renderer);

	return p;
}

void Game::Tick(const Vector2f& move)
{
	m_mode->Tick(move);
}

void Game::Render()
{
	m_mode->Render();
}
