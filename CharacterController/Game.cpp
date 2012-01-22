// Game.cpp
// Nolan Check
// Created 1/21/2012

#include "Game.hpp"

Game::Game()
{ }

GamePtr Game::Create()
{
	GamePtr p(new Game);
	
	p->m_camera = Camera::Create();
	if (!p->m_camera) {
		return NULL;
	}

	p->m_world = World::Create();
	if (!p->m_world) {
		return NULL;
	}

	return p;
}
