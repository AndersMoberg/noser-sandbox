// CharacterTestMode.cpp
// Nolan Check
// Created 3/21/2012

#include "CharacterTestMode.hpp"

#include <sstream>

#include "Camera.hpp"
#include "GLES2Texture.hpp"
#include "Geometry.hpp"
#include "Game.hpp"
#include "MainMenuMode/MainMenuMode.hpp"

namespace CharacterTestMode
{

class MyGameObject : public GameObject
{
public:
	MyGameObject(GameRenderer* renderer)
		: m_renderer(renderer),
		m_wait(0),
		m_bottles(99),
		m_state(0)
	{ }
	virtual void Tick()
	{
		if (m_wait > 0) // Wait
		{
			if (m_text) {
				m_text->Tick();
			}
			--m_wait;
		}
		else if (m_state == 0) // x bottles of beer on the wall, x bottles of beer...
		{
			std::wstringstream ss;
			ss << m_bottles << " bottles of beer on the wall, " << m_bottles << " bottles of beer...";
			m_text.reset(RevealingText::Create(m_renderer,
				ss.str(),
				Rectf(0.0f, 0.0f, (float)m_renderer->GetGLES2Manager()->GetWidth(),
				(float)m_renderer->GetGLES2Manager()->GetHeight())));
			m_wait = Game::TICKS_PER_SEC * 3;
			m_state = 1;
		}
		else if (m_state == 1) // take one down, pass it around...
		{
			m_text.reset(RevealingText::Create(m_renderer,
				L"...take one down, pass it around...",
				Rectf(0.0f, 0.0f, (float)m_renderer->GetGLES2Manager()->GetWidth(),
				(float)m_renderer->GetGLES2Manager()->GetHeight())));
			m_wait = Game::TICKS_PER_SEC * 3;
			m_state = 2;
		}
		else if (m_state == 2) // ...x bottles of beer on the wall.
		{
			--m_bottles;
			std::wstringstream ss;
			ss << "..." << m_bottles << " bottles of beer on the wall.";
			m_text.reset(RevealingText::Create(m_renderer,
				ss.str(),
				Rectf(0.0f, 0.0f, (float)m_renderer->GetGLES2Manager()->GetWidth(),
				(float)m_renderer->GetGLES2Manager()->GetHeight())));
			m_wait = Game::TICKS_PER_SEC * 3;
			m_state = 0;
		}
	}
	virtual void Render()
	{
		if (m_text) {
			m_text->Render();
		}
	}
private:
	GameRenderer* m_renderer;
	unsigned long m_wait;
	std::unique_ptr<RevealingText> m_text;
	int m_state;
	long m_bottles;
};

CharacterTestMode::CharacterTestMode()
	: m_characterPos(0.0f, 0.0f),
	m_characterSpeed(6.0f),
	m_characterRect(-2.0f, 2.0f, 2.0f, -2.0f)
{ }

CharacterTestMode* CharacterTestMode::Create(Game* game, GameRenderer* renderer)
{
	CharacterTestMode* p(new CharacterTestMode);

	p->m_game = game;
	p->m_renderer = renderer;

	p->m_bgTexture.reset(p->m_renderer->GetGLES2Manager()->CreateTextureFromFile(
		L"C:\\Users\\Public\\Pictures\\Sample Pictures\\Tulips.jpg"));

	p->m_characterTexture.reset(p->m_renderer->GetGLES2Manager()->CreateTextureFromFile(
		L"C:\\Users\\Public\\Pictures\\Sample Pictures\\Jellyfish.jpg"));

	p->m_object.reset(new MyGameObject(p->m_renderer));

	return p;
}

class MainMenuModeSwitcher : public GameModeSwitcher
{
public:
	GameMode* CreateMode(Game* game, GameRenderer* renderer) {
		return MainMenuMode::MainMenuMode::Create(game, renderer);
	}
};

void CharacterTestMode::Tick(const GameInput& input)
{
	if (input.esc)
	{
		m_game->SwitchMode(new MainMenuModeSwitcher);
	}
	else
	{
		Vector2f velocity = input.move * m_characterSpeed;
		m_characterPos += velocity / (float)Game::TICKS_PER_SEC;

		m_object->Tick();
	}
}

void CharacterTestMode::Render()
{
	unsigned int width = m_renderer->GetGLES2Manager()->GetWidth();
	unsigned int height = m_renderer->GetGLES2Manager()->GetHeight();

	glViewport(0, 0, width, height);

	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	Rectf vp(0.0f, 0.0f, (float)width, (float)height);
	Matrix3x2f worldToViewport = m_camera.GetWorldToViewport(vp);
	Matrix3x2f viewportToClip = Matrix3x2f::RectLerp(vp,
		Rectf(-1.0f, 1.0f, 1.0f, -1.0f));
	Matrix3x2f worldToClip = worldToViewport * viewportToClip;

	m_renderer->GetGLES2Manager()->SetTexturedQuadMatrix(worldToClip);
	
	// Draw background image
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_bgTexture->Get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Premultiplied alpha blending
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	glEnable(GL_BLEND);

	m_renderer->GetGLES2Manager()->DrawTexturedQuad(Rectf(-16.0f, 16.0f, 16.0f, -16.0f));

	// Draw character image
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_characterTexture->Get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Premultiplied alpha blending
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	glEnable(GL_BLEND);

	m_renderer->GetGLES2Manager()->DrawTexturedQuad(m_characterRect.Offset(m_characterPos));

	m_object->Render();
}

}
