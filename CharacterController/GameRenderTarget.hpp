// GameRenderTarget.hpp
// Nolan Check
// Created 2/20/2012

#ifndef _GAMERENDERTARGET_HPP
#define _GAMERENDERTARGET_HPP

#include <d2d1.h>
#include <dwrite.h>
#undef DrawText

#include <memory>
#include <string>

#include "Geometry.hpp"
#include "WindowsUtils.hpp"

class GameRenderTarget;
typedef std::shared_ptr<GameRenderTarget> GameRenderTargetPtr;

class GameRenderTarget
{

public:

	~GameRenderTarget();
	static GameRenderTargetPtr Create();

	void SetD2DTarget(ComPtr<ID2D1RenderTarget> target);
	void ReleaseD2DTarget();

	ComPtr<IDWriteFactory> GetDWriteFactory() { return m_dwriteFactory; }
	ComPtr<IDWriteTextFormat> GetDialogTextFormat() { return m_dialogTextFormat; }

	ComPtr<ID2D1RenderTarget> GetD2DTarget() { return m_d2dTarget; }
	ComPtr<ID2D1SolidColorBrush> GetBlackBrush() { return m_blackBrush; }
	ComPtr<ID2D1SolidColorBrush> GetWhiteBrush() { return m_whiteBrush; }

	void DrawText(const std::wstring& text,
		ComPtr<IDWriteTextFormat> textFormat,
		ComPtr<ID2D1Brush> fillBrush,
		ComPtr<ID2D1Brush> strokeBrush,
		float strokeWidth,
		const Rectf& layoutBox);

	void DrawTextLayout(ComPtr<IDWriteTextLayout> textLayout,
		ComPtr<ID2D1Brush> fillBrush, ComPtr<ID2D1Brush> strokeBrush, float strokeWidth,
		const Vector2f& origin);

private:

	GameRenderTarget();

	// Resources independent of D2D target
	ComPtr<IDWriteFactory> m_dwriteFactory;
	ComPtr<IDWriteTextFormat> m_dialogTextFormat;

	// Resources dependent on D2D target
	ComPtr<ID2D1RenderTarget> m_d2dTarget;
	ComPtr<ID2D1SolidColorBrush> m_blackBrush;
	ComPtr<ID2D1SolidColorBrush> m_whiteBrush;

};

#endif
