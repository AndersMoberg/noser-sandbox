// RenderTargetView.hpp
// Nolan Check
// Created 1/11/2012

#ifndef _RENDERTARGETVIEW_HPP
#define _RENDERTARGETVIEW_HPP

#include <memory>

#include "Texture2D.hpp"

namespace D3D11
{

class RenderTargetView;
typedef std::shared_ptr<RenderTargetView> RenderTargetViewPtr;

class RenderTargetView
{

public:

	~RenderTargetView();
	static RenderTargetViewPtr Create(ID3D11Device* pDevice, Texture2DPtr texture);

	ID3D11RenderTargetView* Get() { return m_pRTV; }

private:

	RenderTargetView();

	ID3D11RenderTargetView* m_pRTV;

};

}

#endif
