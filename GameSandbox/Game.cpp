// Game.cpp
// Nolan Check
// Created 3/17/2012

#include "Game.hpp"

const float Game::TICKS_PER_SEC = 3600.0f;

Game::Game()
	: m_characterPos(0.0f, 0.0f),
	m_characterRect(-1.0f, 1.0f, 1.0f, -1.0f)
{ }

GamePtr Game::Create(GameRendererPtr renderer)
{
	GamePtr p(new Game);

	p->m_renderer = renderer;

	p->m_curTick = 0;

	p->m_bgTexture = p->m_renderer->GetGLES2Manager()->CreateTextureFromFile(
		L"C:\\Users\\Public\\Pictures\\Sample Pictures\\Tulips.jpg");

	p->m_characterTexture = p->m_renderer->GetGLES2Manager()->CreateTextureFromFile(
		L"C:\\Users\\Public\\Pictures\\Sample Pictures\\Jellyfish.jpg");

	return p;
}

void Game::Tick()
{
}

void Game::Render()
{
	glViewport(0, 0, m_renderer->GetGLES2Manager()->GetWidth(),
		m_renderer->GetGLES2Manager()->GetHeight());

	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	// Draw background image
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_bgTexture->Get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Premultiplied alpha blending
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	glEnable(GL_BLEND);

	m_renderer->GetGLES2Manager()->DrawTexturedQuad(Rectf(-1.0f, 1.0f, 1.0f, -1.0f));

	// Draw character image
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_characterTexture->Get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Premultiplied alpha blending
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	glEnable(GL_BLEND);

	m_renderer->GetGLES2Manager()->DrawTexturedQuad(m_characterRect);
}
