// SamplerState.cpp
// Nolan Check
// Created 1/6/2012

#include "SamplerState.hpp"

#include "D3D11Utils.hpp"
#include "WindowsUtils.hpp"

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

	CHECK_HR(pDevice->CreateSamplerState(&sd, &p->m_pSamplerState));

	return p;
}

}
