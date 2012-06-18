// CharacterControllerMode.hpp
// Nolan Check
// Created 3/22/2012

#ifndef _CHARACTERCONTROLLERMODE_HPP
#define _CHARACTERCONTROLLERMODE_HPP

#include <memory>

#include <Box2D/Box2D.h>

#include "D2DRenderer.hpp"
#include "Game.hpp"
#include "World.hpp"
#include "RevealingText.hpp"

namespace CharacterControllerMode
{

class CharacterControllerMode : public GameMode
{

public:

	static std::unique_ptr<CharacterControllerMode> create(Game* game);

	void Tick(const GameInput& input);
	void Render();

private:

	CharacterControllerMode();

	void startRevealingText(const std::wstring& str);
	void clearRevealingText();
	
	struct Character
	{
		Vector2f pos;
		float radius;
	};

	typedef std::shared_ptr<Character> CharacterPtr;

	typedef std::list<CharacterPtr> CharacterList;

	Game* m_game;
	D2DRenderer m_renderer;

	Camera m_camera;
	World m_world;

	Vector2f m_intendedVel;
	Vector2f m_actualVel;
	
	CharacterList m_characters;

	CharacterPtr m_playerCharacter;
	CharacterPtr m_npcCharacter;

	std::shared_ptr<b2World> m_b2World;
	b2Body* m_b2Character;
	b2Body* m_b2Npc;

	bool m_wasTouching;

	ComPtr<IDWriteFactory> m_dwriteFactory;
	ComPtr<IDWriteTextFormat> m_textFormat;
	RevealingText::Ptr m_revealingText;

};

}

#endif
