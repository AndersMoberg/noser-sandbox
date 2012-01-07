// BlendState.cpp
// Nolan Check
// Created 1/6/2012

#include "BlendState.hpp"

#include "D3D11Utils.hpp"

namespace D3D11
{

BlendState::BlendState()
	: m_pBlendState(NULL)
{ }

BlendState::~BlendState()
{
	SafeRelease(m_pBlendState);
}

BlendStatePtr BlendState::Create(ID3D11Device* pDevice, const D3D11_BLEND_DESC& bd)
{
	BlendStatePtr p(new BlendState);

	HRESULT hr = pDevice->CreateBlendState(&bd, &p->m_pBlendState);
	if (FAILED(hr)) {
		return NULL;
	}

	return p;
}

}
