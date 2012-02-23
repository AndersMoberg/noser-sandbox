// GameRenderTarget.cpp
// Nolan Check
// Created 2/20/2012

#include "GameRenderTarget.hpp"

#include "WindowsUtils.hpp"

GameRenderTarget::GameRenderTarget()
	: m_pDWriteFactory(NULL),
	m_pDialogTextFormat(NULL),
	m_pD2DTarget(NULL),
	m_pBlackBrush(NULL)
{ }

GameRenderTarget::~GameRenderTarget()
{
	ReleaseD2DTarget();
	SafeRelease(m_pDialogTextFormat);
	SafeRelease(m_pDWriteFactory);
}

GameRenderTargetPtr GameRenderTarget::Create()
{
	GameRenderTargetPtr p(new GameRenderTarget);
	
	CHECK_HR(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory), (IUnknown**)&p->m_pDWriteFactory));

	CHECK_HR(p->m_pDWriteFactory->CreateTextFormat(L"Calibri", NULL,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL, 24.0f, L"en-US", &p->m_pDialogTextFormat));

	return p;
}

void GameRenderTarget::SetD2DTarget(ID2D1RenderTarget* target)
{
	if (m_pD2DTarget != target)
	{
		m_pD2DTarget = target;

		// Create target-specific resources

		CHECK_HR(m_pD2DTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black), &m_pBlackBrush));
	}
}

void GameRenderTarget::ReleaseD2DTarget()
{
	// Release target-specific resources
	SafeRelease(m_pBlackBrush);
	m_pD2DTarget = NULL;
}

class OutlinedTextRenderer : public IDWriteTextRenderer
{

public:

	OutlinedTextRenderer(ID2D1RenderTarget* d2dTarget,
		ID2D1Brush* strokeBrush, float strokeWidth, ID2D1Brush* fillBrush)
		: m_refCount(0), m_d2dTarget(d2dTarget), m_strokeBrush(strokeBrush),
		m_strokeWidth(strokeWidth), m_fillBrush(fillBrush)
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
			glyphRun->glyphIndices,
			glyphRun->glyphAdvances,
			glyphRun->glyphOffsets,
			glyphRun->glyphCount,
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

		if (m_fillBrush)
		{
			m_d2dTarget->FillGeometry(transGeom, m_fillBrush);
		}

		if (m_strokeBrush)
		{
			m_d2dTarget->DrawGeometry(transGeom, m_strokeBrush, m_strokeWidth);
		}

		SafeRelease(transGeom);

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
	ID2D1RenderTarget* m_d2dTarget; // NOT owned
	ID2D1Brush* m_strokeBrush; // NOT owned
	float m_strokeWidth;
	ID2D1Brush* m_fillBrush; // NOT owned

};

void GameRenderTarget::DrawText(const std::wstring& text,
	IDWriteTextFormat* textFormat,
	ID2D1Brush* strokeBrush,
	float strokeWidth,
	ID2D1Brush* fillBrush,
	const Rectf& layoutBox)
{
	IDWriteTextLayout* textLayout = NULL;
	CHECK_HR(m_pDWriteFactory->CreateTextLayout(
		text.c_str(), text.size(), textFormat,
		layoutBox.right - layoutBox.left,
		layoutBox.bottom - layoutBox.top,
		&textLayout));

	OutlinedTextRenderer renderer(m_pD2DTarget, strokeBrush, strokeWidth, fillBrush);
	// XXX: Code assumes that renderer is not saved by textLayout->Draw
	CHECK_HR(textLayout->Draw(NULL, &renderer, layoutBox.left, layoutBox.top));

	SafeRelease(textLayout);
}
