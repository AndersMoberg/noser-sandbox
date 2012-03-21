// Game.hpp
// Nolan Check
// Created 3/17/2012

#ifndef _GAME_HPP
#define _GAME_HPP

#include <memory>

#include "Camera.hpp"
#include "GameRenderer.hpp"
#include "RevealingText.hpp"

class Game;
typedef std::shared_ptr<Game> GamePtr;

class GameObject;
typedef std::shared_ptr<GameObject> GameObjectPtr;

class GameObject
{
public:
	virtual ~GameObject() { }
	virtual void Tick() = 0;
	virtual void Render() = 0;
};

class Game
{

public:

	static const unsigned long long TICKS_PER_SEC;

	static GamePtr Create(GameRendererPtr renderer);

	void Tick(const Vector2f& move);
	void Render();

private:

	Game();

	GameRendererPtr m_renderer;

	CameraPtr m_camera;

	GLES2TexturePtr m_bgTexture;

	Vector2f m_characterPos;
	float m_characterSpeed;
	Rectf m_characterRect;
	GLES2TexturePtr m_characterTexture;

	GameObjectPtr m_object;

};

#endif
