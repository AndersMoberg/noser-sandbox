// CharacterControllerMode.hpp
// Nolan Check
// Created 3/22/2012

#ifndef _CHARACTERCONTROLLERMODE_HPP
#define _CHARACTERCONTROLLERMODE_HPP

#include <memory>

#include <Box2D/Box2D.h>

#include "Buttons.hpp"
#include "D2DRenderer.hpp"
#include "Game.hpp"
#include "World.hpp"
#include "RevealingText.hpp"

namespace CharacterControllerMode
{

class CharacterControllerMode : public GameMode
{

public:

	typedef std::shared_ptr<CharacterControllerMode> Ptr;

	static Ptr create(Game* game);

	void Tick(const GameInput& input);
	void Render();

private:

	CharacterControllerMode();

	void tickWalking(const GameInput& input);

	static const std::wstring TALK_SCRIPT[];
	int m_talkScriptPtr;
	enum TalkState
	{
		TALKSTATE_RUNNING,
		TALKSTATE_SAYING,
		TALKSTATE_SLEEPING,
		TALKSTATE_WAITING
	};
	TalkState m_talkState;
	int m_talkSleepCount;

	Buttons::Ptr m_buttons;

	void interpretTalkScript();
	void enterTalking();
	void tickTalking(const GameInput& input);

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
	D2DRenderer::Ptr m_renderer;

	Camera m_camera;
	World m_world;

	enum State
	{
		STATE_WALKING,
		STATE_TALKING
	};
	State m_state;

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
