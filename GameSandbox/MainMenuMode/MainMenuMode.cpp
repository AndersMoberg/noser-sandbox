// MainMenuMode.cpp
// Nolan Check
// Created 3/21/2012

#include "MainMenuMode.hpp"

#include <list>

#include "WindowsUtils.hpp"

namespace MainMenuMode
{

MainMenuMode::MainMenuMode()
{ }

MainMenuMode* MainMenuMode::Create(GameRenderer* renderer)
{
	MainMenuMode* p = new MainMenuMode;

	p->m_renderer = renderer;
	p->m_d2dLayer.Create(renderer);

	p->AddOption(L"Character Test");
	p->AddOption(L"Exit");

	return p;
}

void MainMenuMode::Tick(const Vector2f& move)
{
}

void MainMenuMode::Render()
{
	ComPtr<ID2D1RenderTarget> d2dTarget = m_d2dLayer.GetD2DTarget();

	d2dTarget->BeginDraw();

	ComPtr<ID2D1SolidColorBrush> whiteBrush;
	CHECK_HR(d2dTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::White), whiteBrush.Receive()));
	ComPtr<ID2D1SolidColorBrush> blackBrush;
	CHECK_HR(d2dTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black), blackBrush.Receive()));

	for (OptionList::const_iterator it = m_options.begin();
		it != m_options.end(); ++it)
	{
		m_d2dLayer.DrawOutlinedTextLayout(it->textLayout,
			whiteBrush, blackBrush, 1.0f, Vector2f(100.0f, 100.0f));
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

void MainMenuMode::AddOption(const std::wstring& label)
{
	Option newOption;
	CHECK_HR(m_renderer->GetDWriteFactory()->CreateTextLayout(
		label.c_str(), label.size(), m_renderer->GetDefaultTextFormat(),
		(float)m_renderer->GetGLES2Manager()->GetWidth(),
		(float)m_renderer->GetGLES2Manager()->GetHeight(),
		newOption.textLayout.Receive()));
	m_options.push_back(newOption);
}

}
