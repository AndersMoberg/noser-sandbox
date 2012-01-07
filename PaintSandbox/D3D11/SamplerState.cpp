// SamplerState.cpp
// Nolan Check
// Created 1/6/2012

#include "SamplerState.hpp"

#include "D3D11Utils.hpp"

namespace D3D11
{

SamplerState::SamplerState()
	: m_pSamplerState(NULL)
{ }

SamplerState::~SamplerState()
{
	SafeRelease(m_pSamplerState);
}

SamplerStatePtr SamplerState::Create(ID3D11Device* pDevice, const D3D11_SAMPLER_DESC& sd)
{
	SamplerStatePtr p(new SamplerState);

	HRESULT hr = pDevice->CreateSamplerState(&sd, &p->m_pSamplerState);
	if (FAILED(hr)) {
		return NULL;
	}

	return p;
}

}
