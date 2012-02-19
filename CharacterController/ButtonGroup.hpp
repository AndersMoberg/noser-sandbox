// ButtonGroup.hpp
// Nolan Check
// Created 2/19/2012

#ifndef _BUTTONGROUP_HPP
#define _BUTTONGROUP_HPP

#include <d2d1.h>

#include <vector>
#include <memory>

#include "Button.hpp"

class ButtonGroup;
typedef std::shared_ptr<ButtonGroup> ButtonGroupPtr;

class ButtonGroup
{

public:

	static ButtonGroupPtr Create(const Vector2f& anchor);

	void AddButton(const std::wstring& label);

	void Render(GamePtr game, ID2D1RenderTarget* target);

private:

	ButtonGroup();

	typedef std::vector<ButtonPtr> ButtonList;
	ButtonList m_buttons;

	Vector2f m_anchor;
	int m_selected;

};

#endif
