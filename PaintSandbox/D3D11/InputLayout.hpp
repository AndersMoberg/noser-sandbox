// InputLayout.hpp
// Nolan Check
// Created 1/6/2012

#ifndef _INPUTLAYOUT_HPP
#define _INPUTLAYOUT_HPP

#include <D3D11.h>

#include <memory>

namespace D3D11
{

class InputLayout;
typedef std::shared_ptr<InputLayout> InputLayoutPtr;

class InputLayout
{

public:

	~InputLayout();
	static InputLayoutPtr Create(ID3D11Device* pDevice,
		const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT numElements,
		const void* pShaderBytecode, SIZE_T bytecodeSize);

	ID3D11InputLayout* Get() { return m_pInputLayout; }

private:

	InputLayout();

	ID3D11InputLayout* m_pInputLayout;

};

}

#endif
