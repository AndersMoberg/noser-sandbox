// RevealingText.cpp
// Nolan Check
// Created 2/22/2012

#include "RevealingText.hpp"

#include "WindowsUtils.hpp"

RevealingText::RevealingText()
	: m_textLayout(NULL)
{ }

RevealingText::~RevealingText()
{
	SafeRelease(m_textLayout);
}

RevealingTextPtr RevealingText::Create(GameRenderTargetPtr target,
	const std::wstring& text, const Rectf& layoutBox)
{
	RevealingTextPtr p(new RevealingText);

	p->m_text = text;
	p->m_layoutBox = layoutBox;

	IDWriteFactory* dwFactory = target->GetDWriteFactory();
	CHECK_HR(dwFactory->CreateTextLayout(
		text.c_str(), text.size(), target->GetDialogTextFormat(),
		layoutBox.right - layoutBox.left,
		layoutBox.bottom - layoutBox.top,
		&p->m_textLayout));

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

class DoNothingObject : public IUnknown
{
public:
	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject)
	{
		if (ppvObject == NULL) {
			return E_POINTER;
		}
		*ppvObject = NULL;
		if (riid == IID_IUnknown) {
			*ppvObject = this;
			return S_OK;
		} else {
			return E_NOINTERFACE;
		}
	}
	STDMETHOD_(ULONG, AddRef)()
	{
		return InterlockedIncrement(&m_refCount);
	}
	STDMETHOD_(ULONG, Release)()
	{
		ULONG refCount = InterlockedDecrement(&m_refCount);
		if (refCount == 0) {
			delete this;
		}
		return refCount;
	}
	static DoNothingObject* Create()
	{
		return new DoNothingObject;
	}
private:
	DoNothingObject()
		: m_refCount(1)
	{ }
	ULONG m_refCount;
};

void RevealingText::Render(GameRenderTargetPtr target)
{
	DWRITE_TEXT_RANGE fullRange = { 0, m_text.size() };
	m_textLayout->SetDrawingEffect(NULL, fullRange);

	DWRITE_TEXT_RANGE hiddenRange = { m_progress, m_text.size() - m_progress };
	DoNothingObject* nullDrawingEffect = DoNothingObject::Create();
	m_textLayout->SetDrawingEffect(nullDrawingEffect, hiddenRange);
	target->DrawTextLayout(m_textLayout, target->GetBlackBrush(), 0.5f, NULL,
		m_layoutBox.UpperLeft());
	nullDrawingEffect->Release();
}
