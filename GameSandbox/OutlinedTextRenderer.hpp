// OutlinedTextRenderer.hpp
// Nolan Check
// Created 3/20/2012

#ifndef _OUTLINEDTEXTRENDERER_HPP
#define _OUTLINEDTEXTRENDERER_HPP

#include <D2D1.h>
#include <DWrite.h>

#include "WindowsUtils.hpp"

class OutlinedTextRenderer : public IDWriteTextRenderer
{

public:

	static ComPtr<OutlinedTextRenderer> Create(
		ComPtr<ID2D1RenderTarget> d2dTarget, ComPtr<ID2D1Brush> fillBrush,
		ComPtr<ID2D1Brush> strokeBrush, float strokeWidth);

	// IUnknown implementation

	IFACEMETHOD(QueryInterface)(REFIID riid, void** ppvObject);
	IFACEMETHOD_(ULONG, AddRef)();
	IFACEMETHOD_(ULONG, Release)();

	// IDWritePixelSnapping implementation

	IFACEMETHOD(IsPixelSnappingDisabled)(void* clientDrawingContext,
		BOOL* isDisabled);
	IFACEMETHOD(GetCurrentTransform)(void* clientDrawingContext,
		DWRITE_MATRIX* transform);
	IFACEMETHOD(GetPixelsPerDip)(void* clientDrawingContext,
		FLOAT* pixelsPerDip);

	// IDWriteTextRenderer implementation

	IFACEMETHOD(DrawGlyphRun)(void* clientDrawingContext,
		FLOAT baselineOriginX, FLOAT baselineOriginY,
		DWRITE_MEASURING_MODE measuringMode,
		const DWRITE_GLYPH_RUN* glyphRun,
		const DWRITE_GLYPH_RUN_DESCRIPTION* glyphRunDescription,
		IUnknown* clientDrawingEffect);
	IFACEMETHOD(DrawUnderline)(void* clientDrawingContext,
		FLOAT baselineOriginX, FLOAT baselineOriginY,
		const DWRITE_UNDERLINE* underline,
		IUnknown* clientDrawingEffect);
	IFACEMETHOD(DrawStrikethrough)(void* clientDrawingContext,
		FLOAT baselineOriginX, FLOAT baselineOriginY,
		const DWRITE_STRIKETHROUGH* strikethrough,
		IUnknown* clientDrawingEffect);
	IFACEMETHOD(DrawInlineObject)(void* clientDrawingContext,
		FLOAT originX, FLOAT originY,
		IDWriteInlineObject* inlineObject,
		BOOL isSideways, BOOL isRightToLeft,
		IUnknown* clientDrawingEffect);

private:

	OutlinedTextRenderer(
		ComPtr<ID2D1RenderTarget> d2dTarget, ComPtr<ID2D1Brush> fillBrush,
		ComPtr<ID2D1Brush> strokeBrush, float strokeWidth);
	~OutlinedTextRenderer();

	ULONG m_refCount;
	ComPtr<ID2D1RenderTarget> m_d2dTarget;
	ComPtr<ID2D1Brush> m_fillBrush;
	ComPtr<ID2D1Brush> m_strokeBrush;
	float m_strokeWidth;

};

#endif
