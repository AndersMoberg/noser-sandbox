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
	RectF curRect(pos.x-1.0f, pos.y+1.0f, pos.x+1.0f, pos.y-1.0f);

	if (down)
	{
		Flow(m_prevRect, curRect);
	}

	m_prevRect = curRect;
}

void DrawTool::Flow(const RectF& rc0, const RectF& rc1)
{
	static const int NUM_STEPS = 1024;
	for (int i = 0; i < NUM_STEPS; ++i)
	{
		RectF rc = LerpRect(0.0f, (float)(i-1), rc0, rc1, (float)i);
		m_renderer->RenderCircularGradient(rc);
	}
}
