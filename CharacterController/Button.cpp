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

void Button::Render(GameRenderTargetPtr target)
{
	ID2D1RenderTarget* d2dTarget = target->GetD2DTarget();

	d2dTarget->DrawRectangle(m_rect, target->GetBlackBrush(), m_selected ? 3.0f : 1.0f);
	target->DrawText(m_label,
		target->GetDialogTextFormat(),
		target->GetBlackBrush(),
		ComPtr<ID2D1Brush>(), 0.0f,
		m_rect);
}
