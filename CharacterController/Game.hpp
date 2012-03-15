// Game.hpp
// Nolan Check
// Created 1/21/2012

#ifndef _GAME_HPP
#define _GAME_HPP

#include <memory>

#include <dwrite.h>

#include "Camera.hpp"
#include "World.hpp"
#include "GameRenderTarget.hpp"
#include "RevealingText.hpp"

class Game;
typedef std::shared_ptr<Game> GamePtr;
class Button;
typedef std::shared_ptr<Button> ButtonPtr;
class ButtonGroup;
typedef std::shared_ptr<ButtonGroup> ButtonGroupPtr;

struct Character
{
	Vector2f pos;
	float radius;
};

typedef std::shared_ptr<Character> CharacterPtr;

typedef std::list<CharacterPtr> CharacterList;

class Game
{

public:

	static GamePtr Create();

	void SetD2DTarget(ComPtr<ID2D1RenderTarget> target);
	void ReleaseD2DTarget();

	void Update(const Vector2f& move, bool spaceTrigger);
	void Render();
	
	void RenderButton(ButtonPtr button);

private:

	Game();

	typedef std::list<const Wall*> Collisions;
	Collisions CheckCharacterCollisions(const Character& character, const Vector2f& vel);

	void Step(const Vector2f& move);

	void RenderPrintf(ID2D1RenderTarget* pD2DTarget,
		IDWriteTextFormat* textFormat, const D2D1_RECT_F& layoutRect,
		ID2D1Brush* defaultForegroundBrush,
		LPCWSTR msg, ...);

	bool CanPlayerTalk();

	long long m_frequency;
	long long m_prevTime;

	GameRenderTargetPtr m_renderTarget;

	CameraPtr m_camera;
	WorldPtr m_world;
	CharacterList m_characters;

	CharacterPtr m_playerCharacter;
	CharacterPtr m_npcCharacter;

	// velocities for display
	Vector2f m_intendedVel;
	Vector2f m_actualVel;

	bool m_talking;
	RevealingTextPtr m_revealingText;
	ButtonGroupPtr m_buttonGroup;

};

#endif
