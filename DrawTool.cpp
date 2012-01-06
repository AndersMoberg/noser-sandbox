// DrawTool.cpp
// Nolan Check
// Created 1/3/2012

#include "DrawTool.hpp"

#include "CanvasImage.hpp"
#include "Driver.hpp"

DrawTool::DrawTool()
{ }

DrawToolPtr DrawTool::Create(DriverPtr driver, CanvasImagePtr image)
{
	DrawToolPtr p(new DrawTool);

	p->m_driver = driver;
	p->m_image = image;
	p->m_renderer = p->m_driver->CreateDrawToolRenderer(image);
	p->m_prevRect = RectF(0.0f, 0.0f, 0.0f, 0.0f);
	p->m_flowRate = 1.0f; // 1 unit of ink per canvas length

	return p;
}

void DrawTool::ReceiveCursor(bool down, const Vector2f& pos)
{
	float radius = 0.5f;
	RectF curRect(pos.x-radius, pos.y+radius, pos.x+radius, pos.y-radius);

	if (down)
	{
		Flow(m_prevRect, curRect);
	}

	m_prevRect = curRect;
}

void DrawTool::Flow(const RectF& rc0, const RectF& rc1)
{
	Vector2f center0 = rc0.Center();
	Vector2f center1 = rc1.Center();
	Vector2f to = center1 - center0;
	float totalWeight = to.Length() * m_flowRate;

	const Matrix3x2f& canvasToPixel = m_image->GetCanvasToPixel();
	Vector2f c0Pixel = canvasToPixel.TransformPoint(center0);
	Vector2f c1Pixel = canvasToPixel.TransformPoint(center1);
	Vector2f toPixel = c1Pixel - c0Pixel;
	float numPixels = toPixel.Length();

	int numSteps = (int)ceil(numPixels);
	if (numSteps < 1) {
		numSteps = 1;
	}
	for (int i = 0; i < numSteps; ++i)
	{
		RectF rc = LerpRect(0.0f, (float)numSteps, rc0, rc1, i + 0.5f);
		m_renderer->RenderCircularGradient(rc, totalWeight / numSteps);
	}
}
