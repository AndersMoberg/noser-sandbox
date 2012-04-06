// MainMenuMode.hpp
// Nolan Check
// Created 3/21/2012

#ifndef	_MAINMENUMODE_HPP
#define _MAINMENUMODE_HPP

#include <memory>
#include <list>

#include "D2DRenderer.hpp"
#include "Game.hpp"

namespace MainMenuMode
{

class MainMenuMode : public GameMode
{

public:

	static std::unique_ptr<MainMenuMode> create(Game* game);

	void Tick(const GameInput& input);
	void Render();

private:

	MainMenuMode();

	Game* m_game;

	std::unique_ptr<D2DRenderer> m_renderer;

	ComPtr<IDWriteFactory> m_dwriteFactory;
	ComPtr<IDWriteTextFormat> m_textFormat;

	bool m_upTriggered;
	bool m_downTriggered;

	struct Option
	{
		ComPtr<IDWriteTextLayout> textLayout;
	};
	typedef std::list<Option> OptionList;

	OptionList m_options;
	size_t m_selection;

	void AddOption(const std::wstring& label);

};

}

#endif
