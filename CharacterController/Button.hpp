// Button.hpp
// Nolan Check
// Created 2/19/2012

#ifndef _BUTTON_HPP
#define _BUTTON_HPP

#include <memory>
#include <string>

#include "Geometry.hpp"

class Button;
typedef std::shared_ptr<Button> ButtonPtr;
class Game;
typedef std::shared_ptr<Game> GamePtr;

class Button : public std::enable_shared_from_this<Button>
{

public:

	static ButtonPtr Create(const Rectf& rect, const std::wstring& label);

	template<typename TRenderTarget>
	void Render(GamePtr game, TRenderTarget *target);

	const Rectf& GetRect() const { return m_rect; }
	const std::wstring& GetLabel() const { return m_label; }

private:

	Button();

	Rectf m_rect;
	std::wstring m_label;

};

template<typename TRenderTarget>
void Button::Render(GamePtr game, TRenderTarget *target)
{
	game->RenderButton(shared_from_this(), target);
}

#endif
