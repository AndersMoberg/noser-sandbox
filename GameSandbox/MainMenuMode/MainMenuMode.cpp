// MainMenuMode.cpp
// Nolan Check
// Created 3/21/2012

#include "MainMenuMode.hpp"

#include <list>

#include "CharacterControllerMode/CharacterControllerMode.hpp"
#include "CharacterTestMode/CharacterTestMode.hpp"
#include "StatesMode/StatesMode.hpp"
#include "OutlinedTextRenderer.hpp"
#include "WindowsUtils.hpp"

namespace MainMenuMode
{

MainMenuMode::MainMenuMode()
	: m_selection(0),
	m_upTriggered(false),
	m_downTriggered(false)
{ }

MainMenuMode::Ptr MainMenuMode::create(Game::Ptr game)
{
	MainMenuMode::Ptr p(new MainMenuMode);

	p->m_game = game;

	p->m_renderer = D2DRenderer::create(game->GetHWnd());
	
	CHECK_HR(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory), (IUnknown**)p->m_dwriteFactory.Receive()));

	CHECK_HR(p->m_dwriteFactory->CreateTextFormat(L"Kootenay", NULL,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL, 32.0f, L"en-US", p->m_textFormat.Receive()));

	p->AddOption(L"Character Controller");
	p->AddOption(L"Character Test");
	p->AddOption(L"States Test");
	p->AddOption(L"Exit");

	return p;
}

GameMode::Ptr MainMenuMode::Tick(const GameInput& input)
{
	Game::Ptr game = m_game.lock();
	assert(game);

	if (input.enter)
	{
		switch (m_selection)
		{
		case 0: // Character Controller
			return CharacterControllerMode::CharacterControllerMode::create(game);
		case 1: // Character Test
			return CharacterTestMode::CharacterTestMode::create(game);
		case 2: // States Test
			return StatesMode::StatesMode::create(game);
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

	return shared_from_this();
}

void MainMenuMode::Render()
{
	ComPtr<ID2D1RenderTarget> d2dTarget = m_renderer->GetD2DTarget();

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
		DrawOutlinedTextLayout(d2dTarget, it->textLayout,
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
	// TODO: Handle when EndDraw returns D2DERR_RECREATETARGET
}

void MainMenuMode::AddOption(const std::wstring& label)
{
	ComPtr<ID2D1RenderTarget> d2dTarget = m_renderer->GetD2DTarget();
	D2D1_SIZE_U targetSize = d2dTarget->GetPixelSize();

	Option newOption;
	CHECK_HR(m_dwriteFactory->CreateTextLayout(
		label.c_str(), label.size(), m_textFormat,
		(float)targetSize.width, (float)targetSize.height,
		newOption.textLayout.Receive()));
	newOption.textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

	m_options.push_back(newOption);
}

}
