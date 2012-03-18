// Game.cpp
// Nolan Check
// Created 3/17/2012

#include "Game.hpp"

Game::Game()
{ }

GamePtr Game::Create(GameRendererPtr renderer)
{
	GamePtr p(new Game);

	p->m_renderer = renderer;

	return p;
}
