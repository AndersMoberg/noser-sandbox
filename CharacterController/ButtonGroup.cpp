// ButtonGroup.cpp
// Nolan Check
// Created 2/19/2012

#include "ButtonGroup.hpp"

ButtonGroup::ButtonGroup()
	: m_selected(0),
	m_selectTriggered(false)
{ }

ButtonGroupPtr ButtonGroup::Create(const Vector2f& anchor)
{
	ButtonGroupPtr p(new ButtonGroup);

	p->m_anchor = anchor;

	return p;
}

void ButtonGroup::AddButton(const std::wstring& label)
{
	ButtonPtr newButton = Button::Create(Rectf(), label);
	m_buttons.push_back(newButton);
}

static const Vector2f DEFAULT_BUTTON_SIZE(80.0f, 50.0f);

void ButtonGroup::Update(const Vector2f& move)
{
	if (!m_selectTriggered)
	{
		if (move.x < -0.5f)
		{
			// Move left
			if (m_selected > 0) {
				--m_selected;
			} else {
				m_selected = m_buttons.size()-1;
			}
			m_selectTriggered = true;
		}
		else if (move.x > 0.5f)
		{
			// Move right
			if (m_selected < m_buttons.size()-1) {
				++m_selected;
			} else {
				m_selected = 0;
			}
			m_selectTriggered = true;
		}
	}
	else
	{
		if (move.x >= -0.5f && move.x <= 0.5f) {
			m_selectTriggered = false;
		}
	}
}

void ButtonGroup::Render(GamePtr game)
{
	float totalWidth = m_buttons.size() * DEFAULT_BUTTON_SIZE.x;
	float totalHeight = DEFAULT_BUTTON_SIZE.y;

	Vector2f topLeft = m_anchor - Vector2f(totalWidth/2.0f, totalHeight/2.0f);

	int buttonNum = 0;
	for (ButtonList::const_iterator it = m_buttons.begin();
		it != m_buttons.end(); ++it)
	{
		Rectf loc(
			topLeft.x + DEFAULT_BUTTON_SIZE.x * buttonNum,
			topLeft.y,
			topLeft.x + DEFAULT_BUTTON_SIZE.x * (buttonNum+1),
			topLeft.y + DEFAULT_BUTTON_SIZE.y);
		(*it)->SetSelected(buttonNum == m_selected);
		++buttonNum;

		(*it)->SetRect(loc);
		(*it)->Render(game);
	}
}
