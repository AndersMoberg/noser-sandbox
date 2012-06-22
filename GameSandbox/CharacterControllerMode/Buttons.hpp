// Buttons.hpp
// Nolan Check
// Created 6/21/2012

#ifndef _BUTTONS_HPP
#define _BUTTONS_HPP

#include <list>

#include "Game.hpp"
#include "WindowsUtils.hpp"

namespace CharacterControllerMode
{

class Buttons
{

public:

	typedef std::shared_ptr<Buttons> Ptr;

	static Ptr create();

	void addChoice(const std::wstring& text);

	void tick(const GameInput& input);
	void render(ComPtr<ID2D1RenderTarget> target);

private:

	Buttons();

	struct Choice
	{
		std::wstring text;
		Rectf rect;
	};
	typedef std::list<Choice> ChoiceList;
	ChoiceList m_choices;

};

}

#endif
