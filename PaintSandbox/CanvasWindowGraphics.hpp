// CanvasWindowGraphics.hpp
// Nolan Check
// Created 12/26/2011

#ifndef _CANVASWINDOWGRAPHICS_HPP
#define _CANVASWINDOWGRAPHICS_HPP

#include <memory>

#include "CanvasImage.hpp"

class CanvasWindowGraphics;
typedef std::shared_ptr<CanvasWindowGraphics> CanvasWindowGraphicsPtr;

class CanvasWindowGraphics
{
public:
	virtual ~CanvasWindowGraphics() { }
	virtual void OnWMSize() = 0;
	virtual void OnWMPaint() = 0;
	virtual void SetCanvasImage(CanvasImagePtr image) = 0;
};

#endif
