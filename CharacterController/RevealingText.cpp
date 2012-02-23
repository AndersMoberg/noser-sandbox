// RevealingText.cpp
// Nolan Check
// Created 2/22/2012

#include "RevealingText.hpp"

RevealingText::RevealingText()
{ }

RevealingTextPtr RevealingText::Create(const std::wstring& text,
	const Rectf& layoutBox)
{
	RevealingTextPtr p(new RevealingText);

	p->m_text = text;
	p->m_layoutBox = layoutBox;

	return p;
}

void RevealingText::Start(long long startTime, long long timerFreq, float charsPerSec)
{
	m_progress = 0;
	m_startTime = startTime;
	m_timerFreq = timerFreq;
	m_charsPerSec = charsPerSec;
}

void RevealingText::Update(long long curTime)
{
	long long tickProgress = curTime - m_startTime;
	float secsProgress = tickProgress / (float)m_timerFreq;
	m_progress = (size_t)(m_charsPerSec * secsProgress);
	if (m_progress > m_text.size()) {
		m_progress = m_text.size();
	}
}

void RevealingText::Render(GameRenderTargetPtr target)
{
	target->DrawText(m_text.substr(0, m_progress), target->GetDialogTextFormat(),
		target->GetBlackBrush(), 0.5f, NULL, m_layoutBox);
}
