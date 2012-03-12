// RevealingText.cpp
// Nolan Check
// Created 2/22/2012

#include "RevealingText.hpp"

#include "WindowsUtils.hpp"

GlyphRecording::GlyphRecording()
{ }

GlyphRecording::~GlyphRecording()
{
	for (GeometryList::iterator it = m_glyphGeometry.begin();
		it != m_glyphGeometry.end(); ++it)
	{
		SafeRelease(*it);
	}
	m_glyphGeometry.clear();
}

GlyphRecordingPtr GlyphRecording::Create()
{
	GlyphRecordingPtr p(new GlyphRecording);

	return p;
}

void GlyphRecording::AddGlyphGeometry(ID2D1Geometry* geom)
{
	m_glyphGeometry.push_back(geom);
}

void GlyphRecording::Draw(ID2D1RenderTarget* target, ID2D1Brush* fillBrush,
	ID2D1Brush* strokeBrush, float strokeWidth, size_t progress)
{
	size_t i = 0;
	for (GeometryList::const_iterator it = m_glyphGeometry.begin();
		it != m_glyphGeometry.end(); ++it)
	{
		if (i >= progress)
			break;

		if (fillBrush)
		{
			target->FillGeometry(*it, fillBrush);
		}
		if (strokeBrush)
		{
			target->DrawGeometry(*it, strokeBrush, strokeWidth);
		}

		++i;
	}
}

RevealingText::RevealingText()
	: m_textLayout(NULL)
{ }

RevealingText::~RevealingText()
{
	SafeRelease(m_textLayout);
}

class GlyphRecorder : public IDWriteTextRenderer
{

public:

	GlyphRecorder(GlyphRecordingPtr glyphRecording, ID2D1RenderTarget* d2dTarget)
		: m_refCount(0),
		m_glyphRecording(glyphRecording),
		m_d2dTarget(d2dTarget)
	{ }

	// IUnknown implementation

	IFACEMETHOD(QueryInterface)(REFIID riid, void** ppvObject)
	{
		if (__uuidof(IDWriteTextRenderer) == riid) {
			*ppvObject = this;
		} else if (__uuidof(IDWritePixelSnapping) == riid) {
			*ppvObject = this;
		} else if (__uuidof(IUnknown) == riid) {
			*ppvObject = this;
		} else {
			*ppvObject = NULL;
			return E_FAIL;
		}

		AddRef();
		return S_OK;
	}

	IFACEMETHOD_(ULONG, AddRef)()
	{
		return InterlockedIncrement(&m_refCount);
	}

	IFACEMETHOD_(ULONG, Release)()
	{
		return InterlockedDecrement(&m_refCount);
	}

	// IDWritePixelSnapping implementation

	IFACEMETHOD(IsPixelSnappingDisabled)(void* clientDrawingContext,
		BOOL* isDisabled)
	{
		*isDisabled = FALSE;
		return S_OK;
	}

	IFACEMETHOD(GetCurrentTransform)(void* clientDrawingContext,
		DWRITE_MATRIX* transform)
	{
		m_d2dTarget->GetTransform((D2D1_MATRIX_3X2_F*)transform);
		return S_OK;
	}

	IFACEMETHOD(GetPixelsPerDip)(void* clientDrawingContext,
		FLOAT* pixelsPerDip)
	{
		float x, y;
		m_d2dTarget->GetDpi(&x, &y);
		*pixelsPerDip = x / 96.0f;
		return S_OK;
	}

	// IDWriteTextRenderer implementation

	IFACEMETHOD(DrawGlyphRun)(void* clientDrawingContext,
		FLOAT baselineOriginX, FLOAT baselineOriginY,
		DWRITE_MEASURING_MODE measuringMode,
		const DWRITE_GLYPH_RUN* glyphRun,
		const DWRITE_GLYPH_RUN_DESCRIPTION* glyphRunDescription,
		IUnknown* clientDrawingEffect)
	{
		HRESULT hr = S_OK;

		ID2D1Factory* d2dFactory = NULL;
		m_d2dTarget->GetFactory(&d2dFactory);

		for (UINT32 i = 0; i < glyphRun->glyphCount; ++i)
		{
			ID2D1PathGeometry* geom = NULL;
			hr = d2dFactory->CreatePathGeometry(&geom);
			if (FAILED(hr)) {
				return hr;
			}

			ID2D1GeometrySink* sink = NULL;
			hr = geom->Open(&sink);
			if (FAILED(hr)) {
				SafeRelease(geom);
				return hr;
			}

			hr = glyphRun->fontFace->GetGlyphRunOutline(
				glyphRun->fontEmSize,
				&glyphRun->glyphIndices[i],
				&glyphRun->glyphAdvances[i],
				&glyphRun->glyphOffsets[i],
				1,
				glyphRun->isSideways,
				glyphRun->bidiLevel % 2,
				sink);
			if (FAILED(hr)) {
				SafeRelease(sink);
				SafeRelease(geom);
				return hr;
			}

			hr = sink->Close();
			if (FAILED(hr)) {
				SafeRelease(sink);
				SafeRelease(geom);
				return hr;
			}

			SafeRelease(sink);

			D2D1::Matrix3x2F matrix = D2D1::Matrix3x2F::Translation(
				baselineOriginX, baselineOriginY);

			ID2D1TransformedGeometry* transGeom = NULL;
			hr = d2dFactory->CreateTransformedGeometry(geom, matrix, &transGeom);
			if (FAILED(hr)) {
				SafeRelease(geom);
				return hr;
			}

			SafeRelease(geom);

			m_glyphRecording->AddGlyphGeometry(transGeom);
		}

		return hr;
	}

	IFACEMETHOD(DrawUnderline)(void* clientDrawingContext,
		FLOAT baselineOriginX, FLOAT baselineOriginY,
		const DWRITE_UNDERLINE* underline,
		IUnknown* clientDrawingEffect)
	{
		return E_NOTIMPL;
	}

	IFACEMETHOD(DrawStrikethrough)(void* clientDrawingContext,
		FLOAT baselineOriginX, FLOAT baselineOriginY,
		const DWRITE_STRIKETHROUGH* strikethrough,
		IUnknown* clientDrawingEffect)
	{
		return E_NOTIMPL;
	}

	IFACEMETHOD(DrawInlineObject)(void* clientDrawingContext,
		FLOAT originX, FLOAT originY,
		IDWriteInlineObject* inlineObject,
		BOOL isSideways, BOOL isRightToLeft,
		IUnknown* clientDrawingEffect)
	{
		return E_NOTIMPL;
	}

private:

	unsigned long m_refCount;
	GlyphRecordingPtr m_glyphRecording;
	ID2D1RenderTarget* m_d2dTarget; // NOT owned

};

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

void RevealingText::Render(GameRenderTargetPtr target)
{
	//target->DrawTextLayout(m_textLayout, target->GetBlackBrush(), 0.5f, NULL,
	//	m_layoutBox.UpperLeft());
	if (!m_glyphRecording)
	{
		m_glyphRecording = GlyphRecording::Create();

		GlyphRecorder recorder(m_glyphRecording, target->GetD2DTarget());
		CHECK_HR(m_textLayout->Draw(NULL, &recorder, m_layoutBox.left, m_layoutBox.top));
	}

	m_glyphRecording->Draw(target->GetD2DTarget(), NULL, target->GetBlackBrush(), 0.5f,
		m_progress);
}
