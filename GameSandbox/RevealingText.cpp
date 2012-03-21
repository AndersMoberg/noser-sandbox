// RevealingText.cpp
// Nolan Check
// Created 3/19/2012

#include "RevealingText.hpp"

RevealingText::RevealingText()
{ }

RevealingTextPtr RevealingText::Create(GameRendererPtr renderer,
	const std::wstring& text, const Rectf& layoutBox)
{
	RevealingTextPtr p(new RevealingText);

	p->m_renderer = renderer;

	p->m_text = text;
	p->m_layoutBox = layoutBox;

	ComPtr<IDWriteFactory> dwriteFactory = p->m_renderer->GetDWriteFactory();
	CHECK_HR(dwriteFactory->CreateTextLayout(
		text.c_str(), text.size(), p->m_renderer->GetDefaultTextFormat(),
		layoutBox.right - layoutBox.left, layoutBox.bottom - layoutBox.top,
		p->m_textLayout.Receive()));

	p->m_d2dLayer = D2DLayer::Create(p->m_renderer);

	p->RenderD2DLayer();

	return p;
}

void RevealingText::Tick()
{
}

void RevealingText::Render()
{
	GLES2TexturePtr texture = m_d2dLayer->GetGLTexture();

	glBindTexture(GL_TEXTURE_2D, texture->Get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Premultiplied alpha blending
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	glEnable(GL_BLEND);

	m_renderer->GetGLES2Manager()->SetTexturedQuadMatrix(Matrix3x2f::IDENTITY);
	m_renderer->GetGLES2Manager()->DrawTexturedQuad(Rectf(-1.0f, 1.0f, 1.0f, -1.0f));
}

void RevealingText::RenderD2DLayer()
{
	ComPtr<ID2D1RenderTarget> d2dTarget = m_d2dLayer->GetD2DTarget();

	d2dTarget->BeginDraw();

	d2dTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black, 0.0f));

	ComPtr<ID2D1SolidColorBrush> fillBrush;
	d2dTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), fillBrush.Receive());
	ComPtr<ID2D1SolidColorBrush> strokeBrush;
	d2dTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), strokeBrush.Receive());

	m_d2dLayer->DrawOutlinedTextLayout(m_textLayout, fillBrush, strokeBrush, 1.0f,
		m_layoutBox.UpperLeft());

	HRESULT hr = d2dTarget->EndDraw();
	if (hr == D2DERR_RECREATE_TARGET) {
		m_d2dLayer->DestroyTargetResources();
		// FIXME: What will we do now?
	} else {
		CHECK_HR(hr);
	}
}
