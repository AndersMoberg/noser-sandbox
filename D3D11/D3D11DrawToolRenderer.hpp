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

class D3D11DrawToolRenderer : public DrawToolRenderer
{

public:

	static D3D11DrawToolRendererPtr Create(CanvasImagePtr image);

	// DrawToolRenderer implementation
	virtual void RenderCircularGradient(const RectF& rc);

private:

	D3D11DrawToolRenderer();

	CanvasImagePtr m_image;

};

}

#endif
