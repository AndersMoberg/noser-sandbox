// RevealingText.hpp
// Nolan Check
// Created 2/22/2012

#ifndef _REVEALINGTEXT_HPP
#define _REVEALINGTEXT_HPP

#include <list>
#include <memory>
#include <string>

#include "Geometry.hpp"
#include "GameRenderTarget.hpp"

class RevealingText;
typedef std::shared_ptr<RevealingText> RevealingTextPtr;

class GlyphRecording;
typedef std::shared_ptr<GlyphRecording> GlyphRecordingPtr;

class GlyphRecording
{

public:

	~GlyphRecording();
	static GlyphRecordingPtr Create();

	void AddGlyphGeometry(ID2D1Geometry* geom);
	void Draw(ID2D1RenderTarget* target, ID2D1Brush* fillBrush,
		ID2D1Brush* strokeBrush, float strokeWidth, size_t count);

private:

	GlyphRecording();

	typedef std::list<ID2D1Geometry*> GeometryList;
	GeometryList m_glyphGeometry; // OWNED

};

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
	IDWriteTextLayout* m_textLayout;
	GlyphRecordingPtr m_glyphRecording;

	size_t m_progress;
	long long m_startTime;
	long long m_timerFreq;
	float m_charsPerSec;

};

#endif
