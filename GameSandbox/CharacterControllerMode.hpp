// CharacterControllerMode.hpp
// Nolan Check
// Created 3/22/2012

#ifndef _CHARACTERCONTROLLERMODE_HPP
#define _CHARACTERCONTROLLERMODE_HPP

#include <memory>

#include "Game.hpp"
#include "GameRenderer.hpp"

class CharacterControllerMode;
typedef std::shared_ptr<CharacterControllerMode> CharacterControllerModePtr;

class CharacterControllerMode : public GameMode
{
public:
	static CharacterControllerModePtr Create(GameRendererPtr renderer);
	virtual void Tick(const Vector2f& move) = 0;
	virtual void Render() = 0;
};

#endif
