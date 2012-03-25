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

class CharacterControllerModeImpl;
typedef std::shared_ptr<CharacterControllerModeImpl> CharacterControllerMode;
typedef std::weak_ptr<CharacterControllerModeImpl> CharacterControllerModePtr;

class CharacterControllerModeImpl : public GameMode
{

public:

	static CharacterControllerMode Create(GameRendererPtr renderer);

	void Tick(const Vector2f& move);
	void Render();

private:

	CharacterControllerModeImpl();
	
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

	GameRendererPtr m_renderer;

	D2DLayerPtr m_d2dLayer;

	CameraPtr m_camera;
	WorldPtr m_world;

	Vector2f m_intendedVel;
	Vector2f m_actualVel;
	
	CharacterList m_characters;

	CharacterPtr m_playerCharacter;
	CharacterPtr m_npcCharacter;

};

}

#endif
