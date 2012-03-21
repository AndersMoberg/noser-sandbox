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

class RevealingText;
typedef std::shared_ptr<RevealingText> RevealingTextPtr;

class RevealingText
{

public:

	static RevealingTextPtr Create(GameRendererPtr renderer,
		const std::wstring& text, const Rectf& layoutBox);

	void Tick();
	void Render();

private:

	RevealingText();

	void RenderD2DLayer();

	GameRendererPtr m_renderer;

	std::wstring m_text;
	Rectf m_layoutBox;

	ComPtr<IDWriteTextLayout> m_textLayout;
	D2DLayerPtr m_d2dLayer;

};

#endif
