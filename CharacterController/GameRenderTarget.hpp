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

class GameRenderTarget;
typedef std::shared_ptr<GameRenderTarget> GameRenderTargetPtr;

class GameRenderTarget
{

public:

	~GameRenderTarget();
	static GameRenderTargetPtr Create();

	void SetD2DTarget(ID2D1RenderTarget* target);
	void ReleaseD2DTarget();

	ID2D1RenderTarget* GetD2DTarget() { return m_pD2DTarget; }
	ID2D1SolidColorBrush* GetBlackBrush() { return m_pBlackBrush; }

	IDWriteTextFormat* GetDialogTextFormat() { return m_pDialogTextFormat; }

	void DrawText(const std::wstring& text,
		IDWriteTextFormat* textFormat,
		ID2D1Brush* strokeBrush,
		float strokeWidth,
		ID2D1Brush* fillBrush,
		const Rectf& layoutBox);

private:

	GameRenderTarget();

	// Resources independent of D2D target
	IDWriteFactory* m_pDWriteFactory;
	IDWriteTextFormat* m_pDialogTextFormat;

	// Resources dependent on D2D target
	ID2D1RenderTarget* m_pD2DTarget; // NOT owned
	ID2D1SolidColorBrush* m_pBlackBrush;

};

#endif
