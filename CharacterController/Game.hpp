// Game.hpp
// Nolan Check
// Created 1/21/2012

#ifndef _GAME_HPP
#define _GAME_HPP

#include <memory>

#include <dwrite.h>

#include "Camera.hpp"
#include "World.hpp"

class Game;
typedef std::shared_ptr<Game> GamePtr;

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

	~Game();
	static GamePtr Create();

	void Update(const Vector2f& move);
	void Render(ID2D1RenderTarget* target);

private:

	Game();

	typedef std::list<const Wall*> Collisions;
	Collisions CheckCharacterCollisions(const Character& character, const Vector2f& vel);

	void Step(const Vector2f& move);

	void RenderPrintf(ID2D1RenderTarget* pD2DTarget,
		IDWriteTextFormat* textFormat, const D2D1_RECT_F& layoutRect,
		ID2D1Brush* defaultForegroundBrush,
		LPCWSTR msg, ...);

	IDWriteFactory* m_pDWriteFactory;
	IDWriteTextFormat* m_pDialogTextFormat;

	long long m_frequency;
	long long m_prevTime;

	CameraPtr m_camera;
	WorldPtr m_world;
	CharacterList m_characters;

	CharacterPtr m_playerCharacter;

	// velocities for display
	Vector2f m_intendedVel;
	Vector2f m_actualVel;

};

#endif
