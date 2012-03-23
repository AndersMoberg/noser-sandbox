// CharacterTestMode.hpp
// Nolan Check
// Created 3/21/2012

#ifndef _CHARACTERTESTMODE_HPP
#define _CHARACTERTESTMODE_HPP

#include <memory>

#include "Game.hpp"
#include "GameRenderer.hpp"

class CharacterTestMode;
typedef std::shared_ptr<CharacterTestMode> CharacterTestModePtr;

class CharacterTestMode : public GameMode
{
public:
	static CharacterTestModePtr Create(GameRendererPtr renderer);
	virtual void Tick(const Vector2f& move) = 0;
	virtual void Render() = 0;
};

#endif
