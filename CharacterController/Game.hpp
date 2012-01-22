// Game.hpp
// Nolan Check
// Created 1/21/2012

#ifndef _GAME_HPP
#define _GAME_HPP

#include <memory>

class Game;
typedef std::shared_ptr<Game> GamePtr;

class Game
{

public:

	static GamePtr Create();

private:

	Game();

};

#endif
