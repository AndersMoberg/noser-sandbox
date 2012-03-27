// MainMenuMode.hpp
// Nolan Check
// Created 3/21/2012

#ifndef	_MAINMENUMODE_HPP
#define _MAINMENUMODE_HPP

#include <memory>
#include <list>

#include "Game.hpp"
#include "GameRenderer.hpp"

namespace MainMenuMode
{

class MainMenuMode : public GameMode
{

public:

	static MainMenuMode* Create(GameRenderer* renderer);

	void Tick(const Vector2f& move);
	void Render();
	
private:

	MainMenuMode();

	GameRenderer* m_renderer;

	D2DLayer m_d2dLayer;

	struct Option
	{
		ComPtr<IDWriteTextLayout> textLayout;
	};
	typedef std::list<Option> OptionList;

	OptionList m_options;

	void AddOption(const std::wstring& label);

};

}

#endif
