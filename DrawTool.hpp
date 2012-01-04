// DrawTool.hpp
// Nolan Check
// Created 1/3/2012

#ifndef _DRAWTOOL_HPP
#define _DRAWTOOL_HPP

#include <memory>

#include "CanvasImage.hpp"

class DrawTool;
typedef std::shared_ptr<DrawTool> DrawToolPtr;

class DrawTool
{

public:

	static DrawToolPtr Create(CanvasImagePtr image);

	void ReceiveCursor(const Vector2f& pos);

private:

	CanvasImagePtr m_image;

};

#endif
