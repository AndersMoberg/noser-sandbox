// RevealingText.cpp
// Nolan Check
// Created 3/19/2012

#include "RevealingText.hpp"

#include "OutlinedTextRenderer.hpp"

namespace CharacterControllerMode
{

RevealingText::Ptr RevealingText::create(IDWriteFactory* dwriteFactory,
	IDWriteTextFormat* textFormat, const std::wstring& text,
	const Rectf& layoutBox)
{
	Ptr p(new RevealingText);

	p->m_text = text;
	p->m_layoutBox = layoutBox;

	p->m_progress = 0;
	p->m_ticksPerChar = 2;
	p->m_charCurTick = 0;

	CHECK_HR(dwriteFactory->CreateTextLayout(
		text.c_str(), text.size(), textFormat,
		layoutBox.right - layoutBox.left, layoutBox.bottom - layoutBox.top,
		p->m_textLayout.Receive()));

	return p;
}

bool RevealingText::Tick()
{
	bool needsRerender = false;

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

			needsRerender = true;
		}
		m_charCurTick = 0;
	}

	return needsRerender;
}

void RevealingText::Render(ID2D1RenderTarget* d2dTarget)
{
	if (m_progress > 0)
	{
		ComPtr<ID2D1SolidColorBrush> fillBrush;
		d2dTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), fillBrush.Receive());
		ComPtr<ID2D1SolidColorBrush> strokeBrush;
		d2dTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), strokeBrush.Receive());

		DrawOutlinedTextLayout(d2dTarget, m_textLayout, fillBrush, strokeBrush, 2.0f,
			m_layoutBox.UpperLeft());
	}
}

void RevealingText::finish()
{
	m_progress = m_text.size();

	DWRITE_TEXT_RANGE fullRange = { 0, m_text.size() };
	m_textLayout->SetDrawingEffect(NULL, fullRange);

	DWRITE_TEXT_RANGE hideRange = { m_progress, m_text.size() - m_progress };
	m_textLayout->SetDrawingEffect(DoNothingObject::Create(), hideRange);
}

}
