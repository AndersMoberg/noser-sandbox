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
	
	void Update(const Vector2f& move);
	void Render(ID2D1RenderTarget* target,
		ID2D1Brush* borderBrush,
		ID2D1Brush* textBrush,
		IDWriteTextFormat* labelTextFormat);

private:

	ButtonGroup();

	typedef std::vector<ButtonPtr> ButtonList;
	ButtonList m_buttons;

	bool m_selectTriggered;
	Vector2f m_anchor;
	int m_selected;

};

#endif
