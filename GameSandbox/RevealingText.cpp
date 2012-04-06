// RevealingText.cpp
// Nolan Check
// Created 3/19/2012

#include "RevealingText.hpp"

#include "OutlinedTextRenderer.hpp"

RevealingText::RevealingText(IDWriteFactory* dwriteFactory,
	IDWriteTextFormat* textFormat, const std::wstring& text,
	const Rectf& layoutBox)
{
	m_text = text;
	m_layoutBox = layoutBox;

	m_progress = 0;
	m_ticksPerChar = 60;
	m_charCurTick = 0;

	CHECK_HR(dwriteFactory->CreateTextLayout(
		text.c_str(), text.size(), textFormat,
		layoutBox.right - layoutBox.left, layoutBox.bottom - layoutBox.top,
		m_textLayout.Receive()));
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
	d2dTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black, 0.0f));

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
