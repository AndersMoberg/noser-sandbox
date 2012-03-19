// Game.cpp
// Nolan Check
// Created 3/17/2012

#include "Game.hpp"

const unsigned long long Game::TICKS_PER_SEC = 3600;

Game::Game()
	: m_characterPos(0.0f, 0.0f),
	m_characterSpeed(6.0f),
	m_characterRect(-2.0f, 2.0f, 2.0f, -2.0f)
{ }

GamePtr Game::Create(GameRendererPtr renderer)
{
	GamePtr p(new Game);

	p->m_renderer = renderer;

	p->m_camera = Camera::Create();

	p->m_bgTexture = p->m_renderer->GetGLES2Manager()->CreateTextureFromFile(
		L"C:\\Users\\Public\\Pictures\\Sample Pictures\\Tulips.jpg");

	p->m_characterTexture = p->m_renderer->GetGLES2Manager()->CreateTextureFromFile(
		L"C:\\Users\\Public\\Pictures\\Sample Pictures\\Jellyfish.jpg");

	return p;
}

void Game::Tick(const Vector2f& move)
{
	Vector2f velocity = move * m_characterSpeed;
	m_characterPos += velocity / TICKS_PER_SEC;
}

void Game::Render()
{
	unsigned int width = m_renderer->GetGLES2Manager()->GetWidth();
	unsigned int height = m_renderer->GetGLES2Manager()->GetHeight();

	glViewport(0, 0, width, height);

	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	Rectf vp(0.0f, 0.0f, (float)width, (float)height);
	Matrix3x2f worldToViewport = m_camera->GetWorldToViewport(vp);
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
}
