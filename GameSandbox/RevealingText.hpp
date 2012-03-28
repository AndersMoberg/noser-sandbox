// RevealingText.hpp
// Nolan Check
// Created 3/19/2012

#ifndef	_REVEALINGTEXT_HPP
#define _REVEALINGTEXT_HPP

#include <memory>
#include <string>

#include <DWrite.h>

#include "D2DLayer.hpp"
#include "Geometry.hpp"
#include "GLES2Renderer.hpp"

class RevealingText
{

public:

	static RevealingText* Create(GLES2Renderer* renderer,
		ComPtr<IDWriteFactory> dwriteFactory,
		ComPtr<IDWriteTextFormat> textFormat,
		const std::wstring& text, const Rectf& layoutBox);

	void Tick();
	void Render();

private:

	RevealingText();

	void RenderD2DLayer();

	GLES2Renderer* m_renderer;

	std::wstring m_text;
	Rectf m_layoutBox;
	size_t m_progress;
	unsigned long m_ticksPerChar;
	unsigned long m_charCurTick;

	ComPtr<IDWriteTextLayout> m_textLayout;
	D2DLayer m_d2dLayer;

};

#endif
