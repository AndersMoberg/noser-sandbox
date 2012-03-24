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

class MainMenuMode;
typedef std::shared_ptr<MainMenuMode> MainMenuModePtr;

class MainMenuMode : public GameMode
{
public:
	static MainMenuModePtr Create(GameRendererPtr renderer);
	virtual void Tick(const Vector2f& move) = 0;
	virtual void Render() = 0;
};

}

#endif
