// RevealingText.hpp
// Nolan Check
// Created 3/19/2012

#ifndef	_REVEALINGTEXT_HPP
#define _REVEALINGTEXT_HPP

#include <memory>
#include <string>

#include "D2DLayer.hpp"
#include "GameRenderer.hpp"
#include "Geometry.hpp"

class RevealingText
{

public:

	static RevealingText* Create(GameRenderer* renderer,
		const std::wstring& text, const Rectf& layoutBox);

	void Tick();
	void Render();

private:

	RevealingText();

	void RenderD2DLayer();

	GameRenderer* m_renderer;

	std::wstring m_text;
	Rectf m_layoutBox;
	size_t m_progress;
	unsigned long m_ticksPerChar;
	unsigned long m_charCurTick;

	ComPtr<IDWriteTextLayout> m_textLayout;
	std::unique_ptr<D2DLayer> m_d2dLayer;

};

#endif
