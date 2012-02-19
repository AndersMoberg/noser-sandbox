// Button.cpp
// Nolan Check
// Created 2/19/2012

#include "Button.hpp"

#include "Game.hpp"

Button::Button()
{ }

ButtonPtr Button::Create(const Rectf& rect, const std::wstring& label)
{
	ButtonPtr p(new Button);

	p->m_rect = rect;
	p->m_label = label;

	return p;
}
