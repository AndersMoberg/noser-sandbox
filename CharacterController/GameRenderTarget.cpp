// GameRenderTarget.cpp
// Nolan Check
// Created 2/20/2012

#include "GameRenderTarget.hpp"

#include "WindowsUtils.hpp"

GameRenderTarget::GameRenderTarget()
{ }

GameRenderTarget::~GameRenderTarget()
{ }

GameRenderTargetPtr GameRenderTarget::Create()
{
	GameRenderTargetPtr p(new GameRenderTarget);
	
	CHECK_HR(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory), (IUnknown**)p->m_dwriteFactory.Receive()));

	CHECK_HR(p->m_dwriteFactory->CreateTextFormat(L"Kootenay", NULL,
		DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL, 32.0f, L"en-US", p->m_dialogTextFormat.Receive()));

	return p;
}

void GameRenderTarget::SetD2DTarget(ComPtr<ID2D1RenderTarget> target)
{
	if (m_d2dTarget != target)
	{
		m_d2dTarget = target;

		// Create target-specific resources

		CHECK_HR(m_d2dTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black), m_blackBrush.Receive()));
		CHECK_HR(m_d2dTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::White), m_whiteBrush.Receive()));
	}
}

void GameRenderTarget::ReleaseD2DTarget()
{
	// Release target-specific resources
	m_whiteBrush.Release();
	m_blackBrush.Release();
	m_d2dTarget.Release();
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
		
		// XXX: Specifying a client drawing effect causes this to not draw.
		// Text ranges with a null drawing effect will draw.
		// This is used to implement the revealing text object!
		if (clientDrawingEffect == NULL)
		{
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
	ID2D1RenderTarget* m_d2dTarget; // NOT owned
	ID2D1Brush* m_strokeBrush; // NOT owned
	float m_strokeWidth;
	ID2D1Brush* m_fillBrush; // NOT owned

};

void GameRenderTarget::DrawText(const std::wstring& text,
	ComPtr<IDWriteTextFormat> textFormat,
	ComPtr<ID2D1Brush> fillBrush,
	ComPtr<ID2D1Brush> strokeBrush,
	float strokeWidth,
	const Rectf& layoutBox)
{
	IDWriteTextLayout* textLayout = NULL;
	CHECK_HR(m_dwriteFactory->CreateTextLayout(
		text.c_str(), text.size(), textFormat,
		layoutBox.right - layoutBox.left,
		layoutBox.bottom - layoutBox.top,
		&textLayout));

	OutlinedTextRenderer renderer(m_d2dTarget, strokeBrush, strokeWidth, fillBrush);
	// XXX: Code assumes that renderer is not saved by textLayout->Draw
	CHECK_HR(textLayout->Draw(NULL, &renderer, layoutBox.left, layoutBox.top));

	SafeRelease(textLayout);
}

void GameRenderTarget::DrawTextLayout(ComPtr<IDWriteTextLayout> textLayout,
	ComPtr<ID2D1Brush> fillBrush, ComPtr<ID2D1Brush> strokeBrush, float strokeWidth,
	const Vector2f& origin)
{
	OutlinedTextRenderer renderer(m_d2dTarget, strokeBrush, strokeWidth, fillBrush);
	// XXX: Code assumes that renderer is not saved by textLayout->Draw
	CHECK_HR(textLayout->Draw(NULL, &renderer, origin.x, origin.y));
}
