// MainMenuMode.hpp
// Nolan Check
// Created 3/21/2012

#ifndef	_MAINMENUMODE_HPP
#define _MAINMENUMODE_HPP

#include <memory>
#include <list>

#include <DWrite.h>

#include "D2DRenderer.hpp"
#include "Game.hpp"

namespace MainMenuMode
{

class MainMenuMode : public GameMode, public std::enable_shared_from_this<MainMenuMode>
{

public:

	typedef std::shared_ptr<MainMenuMode> Ptr;

	static Ptr create(Game::Ptr game);

	GameMode::Ptr Tick(const GameInput& input);
	void Render();

private:

	MainMenuMode();

	Game::WeakPtr m_game;

	D2DRenderer::Ptr m_renderer;

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
