// D3D11Utils.cpp
// Nolan Check
// Created 12/30/2011

#include "D3D11Utils.hpp"

#include <D3Dcompiler.h>

namespace D3D11
{

static ID3DBlob* CompileShader(const char* src, const char* entryPoint, const char* target)
{
	HRESULT hr;

	ID3DBlob* pCode = NULL;
	ID3DBlob* pErrorMsgs = NULL;
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
	flags |= D3DCOMPILE_DEBUG;
#endif
	hr = D3DCompile(src, strlen(src), NULL, NULL, NULL, entryPoint, target,
		flags, 0, &pCode, &pErrorMsgs);
	if (pErrorMsgs != NULL)
	{
		OutputDebugStringA((LPCSTR)pErrorMsgs->GetBufferPointer());
		pErrorMsgs->Release();
		pErrorMsgs = NULL;
	}
	if (FAILED(hr)) {
		SafeRelease(pCode);
		return NULL;
	}

	return pCode;
}

VertexShaderPtr CreateVertexShaderFromCode(ID3D11Device* pDevice,
	const char* src, const char* entryPoint, const char* target,
	const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT numElements,
	InputLayoutPtr* ppInputLayout)
{
	ID3DBlob* pCode = CompileShader(src, entryPoint, target);
	if (!pCode) {
		return NULL;
	}

	VertexShaderPtr shader = VertexShader::Create(pDevice,
		pCode->GetBufferPointer(), pCode->GetBufferSize());
	if (!shader) {
		SafeRelease(pCode);
		return NULL;
	}

	if (ppInputLayout)
	{
		*ppInputLayout = InputLayout::Create(pDevice, pInputElementDescs,
			numElements, pCode->GetBufferPointer(), pCode->GetBufferSize());
		if (!*ppInputLayout) {
			SafeRelease(pCode);
			return NULL;
		}
	}

	SafeRelease(pCode);
	return shader;
}

PixelShaderPtr CreatePixelShaderFromCode(ID3D11Device* pDevice,
	const char* src, const char* entryPoint, const char* target)
{
	ID3DBlob* pCode = CompileShader(src, entryPoint, target);
	if (!pCode) {
		return NULL;
	}

	PixelShaderPtr shader = PixelShader::Create(pDevice,
		pCode->GetBufferPointer(), pCode->GetBufferSize());
	if (!shader) {
		SafeRelease(pCode);
		return NULL;
	}

	SafeRelease(pCode);
	return shader;
}

}
