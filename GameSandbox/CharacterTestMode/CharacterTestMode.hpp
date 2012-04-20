// CharacterTestMode.hpp
// Nolan Check
// Created 3/21/2012

#ifndef _CHARACTERTESTMODE_HPP
#define _CHARACTERTESTMODE_HPP

#include <memory>

#include "Game.hpp"
#include "GLES2Renderer.hpp"

namespace CharacterTestMode
{

class CharacterTestMode : public GameMode
{

public:

	static std::unique_ptr<CharacterTestMode> create(Game* game);

	virtual void Tick(const GameInput& input);
	virtual void Render();
	
private:

	CharacterTestMode();

	Game* m_game;

	std::unique_ptr<GLES2Renderer> m_renderer;

	Camera m_camera;

	GLES2Texture m_bgTexture;

	Vector2f m_characterPos;
	float m_characterSpeed;
	Rectf m_characterRect;
	GLES2Texture m_characterTexture;

	class MyGameObject;

	std::unique_ptr<MyGameObject> m_object;

};

}

#endif
