// SamplerState.hpp
// Nolan Check
// Created 1/6/2012

#ifndef _SAMPLERSTATE_HPP
#define _SAMPLERSTATE_HPP

#include <D3D11.h>

#include <memory>

namespace D3D11
{

class SamplerState;
typedef std::shared_ptr<SamplerState> SamplerStatePtr;

class SamplerState
{

public:

	~SamplerState();
	static SamplerStatePtr Create(ID3D11Device* pDevice, const D3D11_SAMPLER_DESC& sd);

	ID3D11SamplerState* Get() { return m_pSamplerState; }

private:

	SamplerState();

	ID3D11SamplerState* m_pSamplerState;

};

}

#endif
