// DrawTool.hpp
// Nolan Check
// Created 1/3/2012

#ifndef _DRAWTOOL_HPP
#define _DRAWTOOL_HPP

#include <memory>

#include "Geometry.hpp"

class DrawTool;
typedef std::shared_ptr<DrawTool> DrawToolPtr;
class DrawToolRenderer;
typedef std::shared_ptr<DrawToolRenderer> DrawToolRendererPtr;
class Driver;
typedef std::shared_ptr<Driver> DriverPtr;
class CanvasImage;
typedef std::shared_ptr<CanvasImage> CanvasImagePtr;

class DrawToolRenderer
{
public:
	virtual ~DrawToolRenderer() { }
	virtual void RenderCircularGradient(const RectF& rc) = 0;
};

class DrawTool
{

public:

	static DrawToolPtr Create(DriverPtr driver, CanvasImagePtr image);

	void ReceiveCursor(bool down, const Vector2f& pos);

private:

	DrawTool();

	void Flow(const RectF& rc0, const RectF& rc1);

	DriverPtr m_driver;
	CanvasImagePtr m_image;
	DrawToolRendererPtr m_renderer;

	RectF m_prevRect;
	float m_flowRate;

};

#endif
