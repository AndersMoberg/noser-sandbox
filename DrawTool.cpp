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

	return p;
}

void DrawTool::ReceiveCursor(const Vector2f& pos)
{
	m_renderer->RenderCircularGradient(RectF(pos.x-1.0f, pos.y+1.0f, pos.x+1.0f, pos.y-1.0f));
}
