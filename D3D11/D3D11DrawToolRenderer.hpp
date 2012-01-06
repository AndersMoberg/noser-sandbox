// D3D11DrawToolRenderer.hpp
// Nolan Check
// Create 1/5/2012

#ifndef _D3D11DRAWTOOLRENDERER_HPP
#define _D3D11DRAWTOOLRENDERER_HPP

#include <memory>

#include "DrawTool.hpp"

namespace D3D11
{

class D3D11DrawToolRenderer;
typedef std::shared_ptr<D3D11DrawToolRenderer> D3D11DrawToolRendererPtr;
class D3D11Driver;
typedef std::shared_ptr<D3D11Driver> D3D11DriverPtr;

class D3D11DrawToolRenderer : public DrawToolRenderer
{

public:

	static D3D11DrawToolRendererPtr Create(D3D11DriverPtr driver, CanvasImagePtr image);

	// DrawToolRenderer implementation
	virtual void RenderCircularGradient(const RectF& rc);

private:

	D3D11DrawToolRenderer();

	D3D11DriverPtr m_driver;
	CanvasImagePtr m_image;

};

}

#endif
