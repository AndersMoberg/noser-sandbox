// Button.hpp
// Nolan Check
// Created 2/19/2012

#ifndef _BUTTON_HPP
#define _BUTTON_HPP

#include <memory>
#include <string>

#include "Geometry.hpp"
#include "Game.hpp"
#include "GameRenderTarget.hpp"

class Button;
typedef std::shared_ptr<Button> ButtonPtr;
class Game;
typedef std::shared_ptr<Game> GamePtr;

class Button
{

public:

	static ButtonPtr Create(const Rectf& rect, const std::wstring& label);

	void Render(GameRenderTargetPtr target);

	const Rectf& GetRect() const { return m_rect; }
	void SetRect(const Rectf& rc) { m_rect = rc; }
	const std::wstring& GetLabel() const { return m_label; }
	bool IsSelected() const { return m_selected; }
	void SetSelected(bool sel) { m_selected = sel; }

private:

	Button();

	Rectf m_rect;
	std::wstring m_label;
	bool m_selected;

};

#endif
