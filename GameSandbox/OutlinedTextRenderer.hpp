// OutlinedTextRenderer.hpp
// Nolan Check
// Created 3/20/2012

#ifndef _OUTLINEDTEXTRENDERER_HPP
#define _OUTLINEDTEXTRENDERER_HPP

#include <D2D1.h>
#include <DWrite.h>

#include "Geometry.hpp"
#include "WindowsUtils.hpp"

void DrawOutlinedTextLayout(ComPtr<ID2D1RenderTarget> d2dTarget,
	ComPtr<IDWriteTextLayout> textLayout, ComPtr<ID2D1Brush> fillBrush,
	ComPtr<ID2D1Brush> strokeBrush, float strokeWidth,
	const Vector2f& origin);

#endif
