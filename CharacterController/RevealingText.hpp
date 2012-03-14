// RevealingText.hpp
// Nolan Check
// Created 2/22/2012

#ifndef _REVEALINGTEXT_HPP
#define _REVEALINGTEXT_HPP

#include <memory>
#include <string>

#include "Geometry.hpp"
#include "GameRenderTarget.hpp"

class RevealingText;
typedef std::shared_ptr<RevealingText> RevealingTextPtr;

class RevealingText
{

public:

	~RevealingText();
	static RevealingTextPtr Create(GameRenderTargetPtr target,
		const std::wstring& text, const Rectf& layoutBox);

	void Start(long long startTime, long long timerFreq, float charsPerSec);
	void Update(long long curTime);
	void Render(GameRenderTargetPtr target);

private:

	RevealingText();

	std::wstring m_text;
	Rectf m_layoutBox;
	ComPtr<IDWriteTextLayout> m_textLayout;

	size_t m_progress;
	long long m_startTime;
	long long m_timerFreq;
	float m_charsPerSec;

};

#endif
