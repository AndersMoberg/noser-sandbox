// Buttons.cpp
// Nolan Check
// Created 6/21/2012

#include "Buttons.hpp"

namespace CharacterControllerMode
{

Buttons::Buttons()
	: m_selection(0),
	m_inputTrigger(false)
{ }

Buttons::Ptr Buttons::create(ComPtr<IDWriteTextFormat> textFormat)
{
	Ptr p(new Buttons);

	p->m_textFormat = textFormat;

	return p;
}

void Buttons::addChoice(const std::wstring& text)
{
	int choiceNum = m_choices.size();
	Choice c;
	c.text = text;
	c.rect = Rectf(64.0f + 72.0f*choiceNum, 128.0f, 64.0f+72.0f*choiceNum+64.0f, 128.0f+64.0f);
	m_choices.push_back(c);
}

void Buttons::tick(const GameInput& input)
{
	// FIXME: More sophisticated is needed if more than two buttons are present
	if (!m_inputTrigger && input.move.LengthSquared() >= 0.5f*0.5f) {
		++m_selection;
		if (m_selection >= m_choices.size()) {
			m_selection = 0;
		}
		m_inputTrigger = true;
	} else if (input.move.LengthSquared() < 0.5f*0.5f) {
		m_inputTrigger = false;
	}
}

void Buttons::render(ComPtr<ID2D1RenderTarget> d2dTarget)
{
	ComPtr<ID2D1SolidColorBrush> bgBrush;
	d2dTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::White), bgBrush.Receive());
	ComPtr<ID2D1SolidColorBrush> borderBrush;
	d2dTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black), borderBrush.Receive());

	int cur = 0;
	for (ChoiceList::const_iterator it = m_choices.begin(); it != m_choices.end(); ++it)
	{
		d2dTarget->FillRectangle(it->rect, bgBrush);
		d2dTarget->DrawTextW(it->text.c_str(), it->text.size(), m_textFormat,
			it->rect, borderBrush);
		d2dTarget->DrawRectangle(it->rect, borderBrush,
			cur == m_selection ? 3.0f : 1.0f);

		++cur;
	}
}

}
