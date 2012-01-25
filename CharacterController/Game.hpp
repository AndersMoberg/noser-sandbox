// Game.hpp
// Nolan Check
// Created 1/21/2012

#ifndef _GAME_HPP
#define _GAME_HPP

#include <memory>

#include "Camera.hpp"
#include "World.hpp"

class Game;
typedef std::shared_ptr<Game> GamePtr;

class Game
{

public:

	static GamePtr Create();

	void Update(const Vector2f& move);
	void Render(ID2D1RenderTarget* target);

private:

	Game();

	long long m_frequency;
	long long m_prevTime;

	CameraPtr m_camera;
	WorldPtr m_world;
	Vector2f m_characterPos;
	float m_characterRadius;

	// velocities for display
	Vector2f m_intendedVel;
	Vector2f m_actualVel;

};

#endif
