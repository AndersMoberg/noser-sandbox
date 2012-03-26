// MainMenuMode.hpp
// Nolan Check
// Created 3/21/2012

#ifndef	_MAINMENUMODE_HPP
#define _MAINMENUMODE_HPP

#include <memory>

#include "Game.hpp"
#include "GameRenderer.hpp"

namespace MainMenuMode
{

class MainMenuMode : public GameMode
{
public:
	static MainMenuMode* Create(GameRenderer* renderer);
	virtual void Tick(const Vector2f& move) = 0;
	virtual void Render() = 0;
};

}

#endif
