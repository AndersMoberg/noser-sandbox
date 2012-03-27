// CharacterControllerMode.hpp
// Nolan Check
// Created 3/22/2012

#ifndef _CHARACTERCONTROLLERMODE_HPP
#define _CHARACTERCONTROLLERMODE_HPP

#include <memory>

#include "Game.hpp"
#include "GameRenderer.hpp"
#include "World.hpp"

namespace CharacterControllerMode
{

class CharacterControllerMode : public GameMode
{

public:

	static CharacterControllerMode* Create(GameRenderer* renderer);

	void Tick(const GameInput& input);
	void Render();

private:

	CharacterControllerMode();
	
	struct Character
	{
		Vector2f pos;
		float radius;
	};

	typedef std::shared_ptr<Character> CharacterPtr;

	typedef std::list<CharacterPtr> CharacterList;

	typedef std::list<const Wall*> Collisions;
	Collisions CheckCharacterCollisions(
		const Character& character, const Vector2f& vel);

	GameRenderer* m_renderer;

	D2DLayer m_d2dLayer;

	Camera m_camera;
	World m_world;

	Vector2f m_intendedVel;
	Vector2f m_actualVel;
	
	CharacterList m_characters;

	CharacterPtr m_playerCharacter;
	CharacterPtr m_npcCharacter;

};

}

#endif
