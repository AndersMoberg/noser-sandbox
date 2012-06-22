// Buttons.cpp
// Nolan Check
// Created 6/21/2012

#include "Buttons.hpp"

namespace CharacterControllerMode
{

Buttons::Buttons()
{ }

Buttons::Ptr Buttons::create()
{
	Ptr p(new Buttons);

	return p;
}

void Buttons::addChoice(const std::wstring& text)
{
	int choiceNum = m_choices.size();
	Choice c;
	c.text = text;
	c.rect = Rectf(64.0f + 64.0f*choiceNum, 128.0f, 64.0f+64.0f*choiceNum+64.0f, 128.0f+64.0f);
	m_choices.push_back(c);
}

void Buttons::tick(const GameInput& input)
{
}

void Buttons::render(ComPtr<ID2D1RenderTarget> d2dTarget)
{
	ComPtr<ID2D1SolidColorBrush> bgBrush;
	d2dTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::White), bgBrush.Receive());

	for (ChoiceList::const_iterator it = m_choices.begin(); it != m_choices.end(); ++it)
	{
		d2dTarget->FillRectangle(it->rect, bgBrush);
	}
}

}
