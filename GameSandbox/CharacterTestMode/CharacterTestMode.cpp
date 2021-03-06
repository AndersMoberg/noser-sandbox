// CharacterTestMode.cpp
// Nolan Check
// Created 3/21/2012

#include "CharacterTestMode.hpp"

#include <sstream>

#include "Camera.hpp"
#include "D2DLayer.hpp"
#include "GLES2Texture.hpp"
#include "Geometry.hpp"
#include "Game.hpp"
#include "MainMenuMode/MainMenuMode.hpp"
#include "DropShadow.hpp"
#include "RevealingText.hpp"

namespace CharacterTestMode
{

class CharacterTestMode::MyGameObject
{
public:
	MyGameObject(GLES2Renderer::Ptr renderer)
		: m_renderer(renderer),
		m_wait(0),
		m_bottles(99),
		m_state(0)
	{
		m_d2dLayer.init(m_renderer);

		CHECK_HR(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory), (IUnknown**)m_dwriteFactory.Receive()));

		CHECK_HR(m_dwriteFactory->CreateTextFormat(L"Trebuchet MS", NULL,
			DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL, 36.0f, L"en-US", m_textFormat.Receive()));

		m_dropShadowCommon.init();

		D2D1_SIZE_U size = m_d2dLayer.getD2DTarget()->GetPixelSize();
		m_dropShadow.init(&m_dropShadowCommon, m_d2dLayer.getGLTexture()->get(), size.width, size.height);

		m_textNeedsRerender = true;
	}
	virtual void Tick()
	{
		if (m_wait > 0) // Wait
		{
			if (m_text) {
				m_textNeedsRerender |= m_text->Tick();
			}
			--m_wait;
		}
		else if (m_state == 0) // x bottles of beer on the wall, x bottles of beer...
		{
			std::wstringstream ss;
			ss << m_bottles << " bottles of beer on the wall, " << m_bottles << " bottles of beer...";
			m_text.reset(new RevealingText);
			m_text->init(m_dwriteFactory, m_textFormat,
				ss.str(),
				Rectf(0.0f, 0.0f, (float)m_renderer->GetWidth(),
				(float)m_renderer->GetHeight()));
			m_wait = Game::TICKS_PER_SEC * 3;
			m_state = 1;
		}
		else if (m_state == 1) // take one down, pass it around...
		{
			m_text.reset(new RevealingText);
			m_text->init(m_dwriteFactory, m_textFormat,
				L"...take one down, pass it around...",
				Rectf(0.0f, 0.0f, (float)m_renderer->GetWidth(),
				(float)m_renderer->GetHeight()));
			m_wait = Game::TICKS_PER_SEC * 3;
			m_state = 2;
		}
		else if (m_state == 2) // ...x bottles of beer on the wall.
		{
			--m_bottles;
			std::wstringstream ss;
			ss << "..." << m_bottles << " bottles of beer on the wall.";
			m_text.reset(new RevealingText);
			m_text->init(m_dwriteFactory, m_textFormat,
				ss.str(),
				Rectf(0.0f, 0.0f, (float)m_renderer->GetWidth(),
				(float)m_renderer->GetHeight()));
			m_wait = Game::TICKS_PER_SEC * 3;
			m_state = 0;
		}
	}
	virtual void Render()
	{
		if (m_text)
		{
			GLES2Texture* texture;
			if (m_textNeedsRerender)
			{
				ID2D1RenderTarget* d2dTarget = m_d2dLayer.getD2DTarget();

				D2D1_SIZE_U size = d2dTarget->GetPixelSize();

				d2dTarget->BeginDraw();

				m_text->Render(m_d2dLayer.getD2DTarget());

				HRESULT hr = d2dTarget->EndDraw();
				if (hr == D2DERR_RECREATE_TARGET) {
					m_d2dLayer.recreateTarget();
				} else {
					CHECK_HR(hr);
				}

				m_d2dLayer.transferToGLTexture();
				texture = m_d2dLayer.getGLTexture();
				
				m_dropShadow.generate(Vector2f(-2.0f/size.width, -2.0f/size.height),
					Vector2f(1.0f/size.width, 1.0f/size.height));

				m_textNeedsRerender = false;
			}
			else
			{
				texture = m_d2dLayer.getGLTexture();
			}

			// Render drop shadow
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_dropShadow.getTexture());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			// Premultiplied alpha blending
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);
			glEnable(GL_BLEND);

			m_renderer->SetTexturedQuadMatrix(Matrix3x2f::IDENTITY);
			m_renderer->DrawTexturedQuad(Rectf(-1.0f, 1.0f, 1.0f, -1.0f));

			// Render D2D layer texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture->get());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			// Premultiplied alpha blending
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);
			glEnable(GL_BLEND);

			m_renderer->SetTexturedQuadMatrix(Matrix3x2f::IDENTITY);
			m_renderer->DrawTexturedQuad(Rectf(-1.0f, 1.0f, 1.0f, -1.0f));

		}
	}
private:
	GLES2Renderer::Ptr m_renderer;
	D2DLayer m_d2dLayer;
	DropShadowCommon m_dropShadowCommon;
	DropShadow m_dropShadow;
	ComPtr<IDWriteFactory> m_dwriteFactory;
	ComPtr<IDWriteTextFormat> m_textFormat;
	unsigned long m_wait;
	std::unique_ptr<RevealingText> m_text;
	bool m_textNeedsRerender;
	int m_state;
	long m_bottles;
};

CharacterTestMode::CharacterTestMode()
	: m_characterPos(0.0f, 0.0f),
	m_characterSpeed(6.0f)
{ }

CharacterTestMode::Ptr CharacterTestMode::create(Game::Ptr game)
{
	Ptr p(new CharacterTestMode);

	p->m_game = game;

	p->m_renderer = GLES2Renderer::create(game->GetHWnd());

	p->m_bgTexture = p->m_renderer->createTextureFromFile(
		L"C:\\Users\\Public\\Pictures\\Sample Pictures\\Tulips.jpg");

	p->m_object.reset(new MyGameObject(p->m_renderer));

	p->m_worldRenderer = WorldRenderer::create(p->m_renderer);

	p->m_characterObject = p->m_worldRenderer->addObject(L"TestObject.png", Rectf(-2.0f, 2.0f, 2.0f, -2.0f));

	return p;
}

GameMode::Ptr CharacterTestMode::Tick(const GameInput& input)
{
	Game::Ptr game = m_game.lock();
	assert(game);

	if (input.esc)
	{
		m_renderer.reset();
		return MainMenuMode::MainMenuMode::create(game);
	}
	else
	{
		Vector2f velocity = input.move * m_characterSpeed;
		m_characterPos += velocity / (float)Game::TICKS_PER_SEC;

		m_object->Tick();

		return shared_from_this();
	}
}

void CharacterTestMode::Render()
{
	unsigned int width = m_renderer->GetWidth();
	unsigned int height = m_renderer->GetHeight();

	glViewport(0, 0, width, height);

	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	Rectf vp(0.0f, 0.0f, (float)width, (float)height);
	Matrix3x2f worldToViewport = m_camera.GetWorldToViewport(vp);
	Matrix3x2f viewportToClip = Matrix3x2f::RectLerp(vp,
		Rectf(-1.0f, 1.0f, 1.0f, -1.0f));
	Matrix3x2f worldToClip = worldToViewport * viewportToClip;

	m_renderer->SetTexturedQuadMatrix(worldToClip);
	
	// Draw background image
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_bgTexture->get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Premultiplied alpha blending
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	glEnable(GL_BLEND);

	m_renderer->DrawTexturedQuad(Rectf(-16.0f, 16.0f, 16.0f, -16.0f));

	m_object->Render();

	m_renderer->SetTexturedQuadMatrix(worldToClip);
	m_worldRenderer->setObjectPos(m_characterObject, m_characterPos);
	m_worldRenderer->render();

	m_renderer->Present();
}

}
