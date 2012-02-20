// Button.cpp
// Nolan Check
// Created 2/19/2012

#include "Button.hpp"

#include "Game.hpp"

Button::Button()
	: m_selected(false)
{ }

ButtonPtr Button::Create(const Rectf& rect, const std::wstring& label)
{
	ButtonPtr p(new Button);

	p->m_rect = rect;
	p->m_label = label;

	return p;
}

void Button::Render(ID2D1RenderTarget* target,
	ID2D1Brush* borderBrush,
	ID2D1Brush* textBrush,
	IDWriteTextFormat* labelTextFormat)
{
	target->DrawRectangle(m_rect, borderBrush, m_selected ? 3.0f : 1.0f);
	target->DrawTextW(m_label.c_str(), m_label.size(),
		labelTextFormat, m_rect, textBrush);
}
