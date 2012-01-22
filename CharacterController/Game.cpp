// Game.cpp
// Nolan Check
// Created 1/21/2012

#include "Game.hpp"

Game::Game()
{ }

GamePtr Game::Create()
{
	GamePtr p(new Game);

	return p;
}
