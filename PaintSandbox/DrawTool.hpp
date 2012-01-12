// DrawTool.hpp
// Nolan Check
// Created 1/3/2012

#ifndef _DRAWTOOL_HPP
#define _DRAWTOOL_HPP

#include <memory>

#include "Geometry.hpp"
#include "ExtensibleImage.hpp"

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
	virtual void RenderCircularGradient(const RectF& rc, float weight) = 0;
};

class DrawTool
{

public:

	static DrawToolPtr Create(DriverPtr driver, CanvasImagePtr image,
		ExtensibleImagePtr extensibleImage);

	void ReceiveCursor(bool down, const Vector2f& pos);

private:

	DrawTool();

	void Flow(const RectF& rc0, const RectF& rc1);

	DriverPtr m_driver;
	CanvasImagePtr m_image;
	// TODO: Discard CanvasImage in favor of ExtensibleImage
	ExtensibleImagePtr m_extensibleImage;
	DrawToolRendererPtr m_renderer;

	RectF m_prevRect;
	float m_flowRate;

};

#endif
