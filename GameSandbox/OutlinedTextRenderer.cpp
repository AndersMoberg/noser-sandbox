// OutlinedTextRenderer.cpp
// Nolan Check
// Created 3/20/2012

#include "OutlinedTextRenderer.hpp"

OutlinedTextRenderer::OutlinedTextRenderer(ComPtr<ID2D1RenderTarget> d2dTarget,
	ComPtr<ID2D1Brush> fillBrush, ComPtr<ID2D1Brush> strokeBrush,
	float strokeWidth)
	: m_refCount(1),
	m_d2dTarget(d2dTarget),
	m_fillBrush(fillBrush),
	m_strokeBrush(strokeBrush),
	m_strokeWidth(strokeWidth)
{ }

OutlinedTextRenderer::~OutlinedTextRenderer()
{ }

ComPtr<OutlinedTextRenderer> OutlinedTextRenderer::Create(
	ComPtr<ID2D1RenderTarget> d2dTarget, ComPtr<ID2D1Brush> fillBrush,
	ComPtr<ID2D1Brush> strokeBrush, float strokeWidth)
{
	ComPtr<OutlinedTextRenderer> p;
	*p.Receive() = new OutlinedTextRenderer(d2dTarget, fillBrush, strokeBrush,
		strokeWidth);
	return p;
}

HRESULT OutlinedTextRenderer::QueryInterface(REFIID riid, void** ppvObject)
{
	if (ppvObject == NULL) {
		return E_POINTER;
	}

	if (__uuidof(IDWriteTextRenderer) == riid) {
		*ppvObject = this;
	} else if (__uuidof(IDWritePixelSnapping) == riid) {
		*ppvObject = this;
	} else if (__uuidof(IUnknown) == riid) {
		*ppvObject = this;
	} else {
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

ULONG OutlinedTextRenderer::AddRef()
{
	return InterlockedIncrement(&m_refCount);
}

ULONG OutlinedTextRenderer::Release()
{
	ULONG c = InterlockedDecrement(&m_refCount);
	if (c == 0) {
		delete this;
	}
	return c;
}

HRESULT OutlinedTextRenderer::IsPixelSnappingDisabled(
	void* clientDrawingContext, BOOL* isDisabled)
{
	*isDisabled = TRUE;
	return S_OK;
}

HRESULT OutlinedTextRenderer::GetCurrentTransform(
	void* clientDrawingContext, DWRITE_MATRIX* transform)
{
	m_d2dTarget->GetTransform((D2D1_MATRIX_3X2_F*)transform);
	return S_OK;
}

HRESULT OutlinedTextRenderer::GetPixelsPerDip(
	void* clientDrawingContext, FLOAT* pixelsPerDip)
{
	float x, y;
	m_d2dTarget->GetDpi(&x, &y);
	*pixelsPerDip = x / 96.0f;
	return S_OK;
}

HRESULT OutlinedTextRenderer::DrawGlyphRun(void* clientDrawingContext,
	FLOAT baselineOriginX, FLOAT baselineOriginY,
	DWRITE_MEASURING_MODE measuringMode,
	const DWRITE_GLYPH_RUN* glyphRun,
	const DWRITE_GLYPH_RUN_DESCRIPTION* glyphRunDescription,
	IUnknown* clientDrawingEffect)
{
	HRESULT hr = S_OK;

	// XXX: Specifying any client drawing effect causes this to not draw.
	// This is used to implement the revealing text object.
	if (clientDrawingEffect == NULL)
	{
		ComPtr<ID2D1Factory> d2dFactory;
		m_d2dTarget->GetFactory(d2dFactory.Receive());

		ComPtr<ID2D1PathGeometry> geom;
		hr = d2dFactory->CreatePathGeometry(geom.Receive());
		if (FAILED(hr)) {
			return hr;
		}

		ComPtr<ID2D1GeometrySink> sink;
		hr = geom->Open(sink.Receive());
		if (FAILED(hr)) {
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
			return hr;
		}

		hr = sink->Close();
		if (FAILED(hr)) {
			return hr;
		}

		D2D1::Matrix3x2F matrix = D2D1::Matrix3x2F::Translation(
			baselineOriginX, baselineOriginY);

		ComPtr<ID2D1TransformedGeometry> transGeom;
		hr = d2dFactory->CreateTransformedGeometry(geom, matrix, transGeom.Receive());
		if (FAILED(hr)) {
			return hr;
		}
		
		if (m_strokeBrush)
		{
			m_d2dTarget->DrawGeometry(transGeom, m_strokeBrush, m_strokeWidth);
		}

		if (m_fillBrush)
		{
			m_d2dTarget->FillGeometry(transGeom, m_fillBrush);
		}
	}

	return hr;
}

HRESULT OutlinedTextRenderer::DrawUnderline(void* clientDrawingContext,
	FLOAT baselineOriginX, FLOAT baselineOriginY,
	const DWRITE_UNDERLINE* underline,
	IUnknown* clientDrawingEffect)
{
	return E_NOTIMPL;
}

HRESULT OutlinedTextRenderer::DrawStrikethrough(void* clientDrawingContext,
	FLOAT baselineOriginX, FLOAT baselineOriginY,
	const DWRITE_STRIKETHROUGH* underline,
	IUnknown* clientDrawingEffect)
{
	return E_NOTIMPL;
}

HRESULT OutlinedTextRenderer::DrawInlineObject(void* clientDrawingContext,
	FLOAT originX, FLOAT originY, IDWriteInlineObject* inlineObject,
	BOOL isSideways, BOOL isRightToLeft, IUnknown* clientDrawingEffect)
{
	return E_NOTIMPL;
}
