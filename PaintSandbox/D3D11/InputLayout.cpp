// InputLayout.cpp
// Nolan Check
// Created 1/6/2012

#include "InputLayout.hpp"

#include "D3D11Utils.hpp"

namespace D3D11
{

InputLayout::InputLayout()
	: m_pInputLayout(NULL)
{ }

InputLayout::~InputLayout()
{
	SafeRelease(m_pInputLayout);
}

InputLayoutPtr InputLayout::Create(ID3D11Device* pDevice,
	const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT numElements,
	const void* pShaderBytecode, SIZE_T bytecodeSize)
{
	InputLayoutPtr p(new InputLayout);

	HRESULT hr = pDevice->CreateInputLayout(pInputElementDescs, numElements,
		pShaderBytecode, bytecodeSize, &p->m_pInputLayout);
	if (FAILED(hr)) {
		return NULL;
	}

	return p;
}

}
