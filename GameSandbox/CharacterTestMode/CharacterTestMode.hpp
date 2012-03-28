// CharacterTestMode.hpp
// Nolan Check
// Created 3/21/2012

#ifndef _CHARACTERTESTMODE_HPP
#define _CHARACTERTESTMODE_HPP

#include <memory>

#include "Game.hpp"

namespace CharacterTestMode
{

class CharacterTestMode : public GameMode
{

public:

	static CharacterTestMode* Create(Game* game);

	virtual void Tick(const GameInput& input);
	virtual void Render();
	virtual void Present();
	
private:

	CharacterTestMode();

	Game* m_game;

	std::unique_ptr<GLES2Renderer> m_renderer;

	Camera m_camera;

	std::unique_ptr<GLES2Texture> m_bgTexture;

	Vector2f m_characterPos;
	float m_characterSpeed;
	Rectf m_characterRect;
	std::unique_ptr<GLES2Texture> m_characterTexture;

	std::unique_ptr<GameObject> m_object;

};

}

#endif
