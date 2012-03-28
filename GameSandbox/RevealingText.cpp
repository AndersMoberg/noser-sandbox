// RevealingText.cpp
// Nolan Check
// Created 3/19/2012

#include "RevealingText.hpp"

RevealingText::RevealingText()
{ }

RevealingText* RevealingText::Create(GameRenderer* renderer,
	const std::wstring& text, const Rectf& layoutBox)
{
	RevealingText* p(new RevealingText);

	p->m_renderer = renderer;

	p->m_text = text;
	p->m_layoutBox = layoutBox;

	p->m_progress = 0;
	p->m_ticksPerChar = 60;
	p->m_charCurTick = 0;

	ComPtr<IDWriteFactory> dwriteFactory = p->m_renderer->GetDWriteFactory();
	CHECK_HR(dwriteFactory->CreateTextLayout(
		text.c_str(), text.size(), p->m_renderer->GetDefaultTextFormat(),
		layoutBox.right - layoutBox.left, layoutBox.bottom - layoutBox.top,
		p->m_textLayout.Receive()));

	p->m_d2dLayer.Create(p->m_renderer);

	p->RenderD2DLayer();

	return p;
}

void RevealingText::Tick()
{
	++m_charCurTick;
	if (m_charCurTick >= m_ticksPerChar)
	{
		if (m_progress < m_text.size())
		{
			++m_progress;

			DWRITE_TEXT_RANGE fullRange = { 0, m_text.size() };
			m_textLayout->SetDrawingEffect(NULL, fullRange);

			DWRITE_TEXT_RANGE hideRange = { m_progress, m_text.size() - m_progress };
			m_textLayout->SetDrawingEffect(DoNothingObject::Create(), hideRange);

			RenderD2DLayer();
		}
		m_charCurTick = 0;
	}
}

void RevealingText::Render()
{
	GLES2Texture* texture = m_d2dLayer.GetGLTexture();

	glBindTexture(GL_TEXTURE_2D, texture->Get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Premultiplied alpha blending
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	glEnable(GL_BLEND);

	m_renderer->GetGLES2Renderer()->SetTexturedQuadMatrix(Matrix3x2f::IDENTITY);
	m_renderer->GetGLES2Renderer()->DrawTexturedQuad(Rectf(-1.0f, 1.0f, 1.0f, -1.0f));
}

void RevealingText::RenderD2DLayer()
{
	ComPtr<ID2D1RenderTarget> d2dTarget = m_d2dLayer.GetD2DTarget();

	d2dTarget->BeginDraw();

	d2dTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black, 0.0f));

	if (m_progress > 0)
	{
		ComPtr<ID2D1SolidColorBrush> fillBrush;
		d2dTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), fillBrush.Receive());
		ComPtr<ID2D1SolidColorBrush> strokeBrush;
		d2dTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), strokeBrush.Receive());

		m_d2dLayer.DrawOutlinedTextLayout(m_textLayout, fillBrush, strokeBrush, 2.0f,
			m_layoutBox.UpperLeft());
	}

	HRESULT hr = d2dTarget->EndDraw();
	if (hr == D2DERR_RECREATE_TARGET) {
		m_d2dLayer.DestroyTargetResources();
		// FIXME: What will we do now?
	} else {
		CHECK_HR(hr);
	}
}
