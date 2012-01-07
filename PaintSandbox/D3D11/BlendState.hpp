// BlendState.hpp
// Nolan Check
// Created 1/6/2012

#ifndef _BLENDSTATE_HPP
#define _BLENDSTATE_HPP

#include <D3D11.h>

#include <memory>

namespace D3D11
{

class BlendState;
typedef std::shared_ptr<BlendState> BlendStatePtr;

class BlendState
{

public:

	~BlendState();
	static BlendStatePtr Create(ID3D11Device* pDevice, const D3D11_BLEND_DESC& bd);

	ID3D11BlendState* Get() { return m_pBlendState; }

private:

	BlendState();

	ID3D11BlendState* m_pBlendState;

};

}

#endif
