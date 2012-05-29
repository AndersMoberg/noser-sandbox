// CharacterTestMode.hpp
// Nolan Check
// Created 3/21/2012

#ifndef _CHARACTERTESTMODE_HPP
#define _CHARACTERTESTMODE_HPP

#include <memory>

#include "Game.hpp"
#include "GLES2Renderer.hpp"
#include "WorldRenderer.hpp"

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

	GLES2Renderer m_renderer;

	Camera m_camera;

	std::unique_ptr<GLES2Texture> m_bgTexture;

	Vector2f m_characterPos;
	float m_characterSpeed;
	Rectf m_characterRect;
	std::unique_ptr<GLES2Texture> m_characterTexture;

	class MyGameObject;

	std::unique_ptr<MyGameObject> m_object;

	std::unique_ptr<WorldRenderer> m_worldRenderer;

};

}

#endif
