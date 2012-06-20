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

namespace CharacterControllerMode
{

class RevealingText
{

public:

	typedef std::shared_ptr<RevealingText> Ptr;

	static Ptr create(IDWriteFactory* dwriteFactory, IDWriteTextFormat* textFormat,
		const std::wstring& text, const Rectf& layoutBox);

	bool Tick(); // Returns true if rendering changed (needs re-rendering)
	void Render(ID2D1RenderTarget* d2dTarget);

	void finish();

private:

	std::wstring m_text;
	Rectf m_layoutBox;
	size_t m_progress;
	unsigned long m_ticksPerChar;
	unsigned long m_charCurTick;

	ComPtr<IDWriteTextLayout> m_textLayout;

};

}

#endif
