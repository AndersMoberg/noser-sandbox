// MainMenuMode.cpp
// Nolan Check
// Created 3/21/2012

#include "MainMenuMode.hpp"

#include <list>

#include "CharacterTestMode/CharacterTestMode.hpp"
#include "WindowsUtils.hpp"

namespace MainMenuMode
{

MainMenuMode::MainMenuMode()
	: m_selection(0),
	m_upTriggered(false),
	m_downTriggered(false)
{ }

MainMenuMode* MainMenuMode::Create(Game* game)
{
	MainMenuMode* p = new MainMenuMode;

	p->m_game = game;
	p->m_renderer.reset(GameRenderer::Create(p->m_game->GetHWnd()));

	p->m_d2dLayer.Create(p->m_renderer.get());

	p->AddOption(L"Character Test");
	p->AddOption(L"Exit");

	return p;
}

class CharacterTestModeSwitcher : public GameModeSwitcher
{
public:
	virtual GameMode* CreateMode(Game* game) {
		return CharacterTestMode::CharacterTestMode::Create(game);
	}
};

void MainMenuMode::Tick(const GameInput& input)
{
	if (input.enter)
	{
		switch (m_selection)
		{
		case 0: // Character Test
			m_game->SwitchMode(new CharacterTestModeSwitcher);
			break;
		}
	}
	else if (input.move.y > 0.5f)
	{
		if (!m_upTriggered)
		{
			m_upTriggered = true;
			m_downTriggered = false;
			if (m_selection <= 0) {
				m_selection = m_options.size()-1;
			} else {
				--m_selection;
			}
		}
	}
	else if (input.move.y < -0.5f)
	{
		if (!m_downTriggered)
		{
			m_upTriggered = false;
			m_downTriggered = true;
			if (m_selection >= m_options.size()-1) {
				m_selection = 0;
			} else {
				++m_selection;
			}
		}
	}
	else
	{
		m_upTriggered = false;
		m_downTriggered = false;
	}
}

void MainMenuMode::Render()
{
	ComPtr<ID2D1RenderTarget> d2dTarget = m_d2dLayer.GetD2DTarget();

	d2dTarget->BeginDraw();

	d2dTarget->Clear(D2D1::ColorF(D2D1::ColorF::Gray));

	ComPtr<ID2D1SolidColorBrush> whiteBrush;
	CHECK_HR(d2dTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::White), whiteBrush.Receive()));
	ComPtr<ID2D1SolidColorBrush> blackBrush;
	CHECK_HR(d2dTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black), blackBrush.Receive()));

	Vector2f pos(0.0f, 100.0f);
	size_t num = 0;
	for (OptionList::const_iterator it = m_options.begin();
		it != m_options.end(); ++it)
	{
		m_d2dLayer.DrawOutlinedTextLayout(it->textLayout,
			whiteBrush, blackBrush, 2.0f, pos);

		if (num == m_selection)
		{
			DWRITE_TEXT_METRICS metrics;
			CHECK_HR(it->textLayout->GetMetrics(&metrics));
			Rectf rc = Rectf(metrics.left - 8.0f, metrics.top - 8.0f,
				metrics.left+metrics.width+8.0f, metrics.top+metrics.height+8.0f).Offset(pos);
			d2dTarget->DrawRectangle(rc, blackBrush, 5.0f);
			d2dTarget->DrawRectangle(rc, whiteBrush, 3.0f);
		}

		pos.y += 64.0f;
		++num;
	}

	d2dTarget->EndDraw();

	GLES2Texture* texture = m_d2dLayer.GetGLTexture();

	glBindTexture(GL_TEXTURE_2D, texture->Get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Premultiplied alpha blending
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	glEnable(GL_BLEND);

	m_renderer->GetGLES2Manager()->SetTexturedQuadMatrix(Matrix3x2f::IDENTITY);
	m_renderer->GetGLES2Manager()->DrawTexturedQuad(Rectf(-1.0f, 1.0f, 1.0f, -1.0f));
}

void MainMenuMode::Present()
{
	m_renderer->GetGLES2Manager()->Present();
}

void MainMenuMode::AddOption(const std::wstring& label)
{
	Option newOption;
	CHECK_HR(m_renderer->GetDWriteFactory()->CreateTextLayout(
		label.c_str(), label.size(), m_renderer->GetDefaultTextFormat(),
		(float)m_renderer->GetGLES2Manager()->GetWidth(),
		(float)m_renderer->GetGLES2Manager()->GetHeight(),
		newOption.textLayout.Receive()));
	newOption.textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_options.push_back(newOption);
}

}
