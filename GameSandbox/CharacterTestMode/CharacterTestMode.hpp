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

	typedef std::shared_ptr<CharacterTestMode> Ptr;

	static Ptr create(Game::Ptr game);

	virtual void Tick(const GameInput& input);
	virtual void Render();
	
private:

	CharacterTestMode();

	Game::WeakPtr m_game;

	GLES2Renderer m_renderer;

	Camera m_camera;

	std::unique_ptr<GLES2Texture> m_bgTexture;

	Vector2f m_characterPos;
	float m_characterSpeed;
	WorldRenderer::ObjectHandle m_characterObject;

	class MyGameObject;

	std::unique_ptr<MyGameObject> m_object;

	std::unique_ptr<WorldRenderer> m_worldRenderer;

};

}

#endif
