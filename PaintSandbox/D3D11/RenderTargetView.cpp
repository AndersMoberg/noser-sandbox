// RenderTargetView.cpp
// Nolan Check
// Created 1/11/2012

#include "RenderTargetView.hpp"

#include "D3D11Utils.hpp"
#include "WindowsUtils.hpp"

namespace D3D11
{

RenderTargetView::RenderTargetView()
	: m_pRTV(NULL)
{ }

RenderTargetView::~RenderTargetView()
{
	SafeRelease(m_pRTV);
}

RenderTargetViewPtr RenderTargetView::Create(ID3D11Device* pDevice, Texture2DPtr texture)
{
	RenderTargetViewPtr p(new RenderTargetView);

	CHECK_HR(pDevice->CreateRenderTargetView(texture->Get(), NULL, &p->m_pRTV));

	return p;
}

}
