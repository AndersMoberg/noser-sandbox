// DrawTool.cpp
// Nolan Check
// Created 1/3/2012

#include "DrawTool.hpp"

DrawToolPtr DrawTool::Create(CanvasImagePtr image)
{
	DrawToolPtr p(new DrawTool);

	p->m_image = image;

	return p;
}
