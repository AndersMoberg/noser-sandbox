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

VertexShader::VertexShader()
	: m_pVShader(NULL)
{ }

VertexShader::~VertexShader()
{
	SafeRelease(m_pVShader);
}

VertexShaderPtr VertexShader::Create(ID3D11Device* pDevice,
	const char* src, const char* entryPoint, const char* target,
	const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT numElements,
	ID3D11InputLayout** ppInputLayout)
{
	VertexShaderPtr p(new VertexShader);

	HRESULT hr;

	ID3DBlob* pCode = CompileShader(src, entryPoint, target);
	if (!pCode) {
		return NULL;
	}

	hr = pDevice->CreateVertexShader(pCode->GetBufferPointer(),
		pCode->GetBufferSize(), NULL, &p->m_pVShader);
	if (FAILED(hr)) {
		SafeRelease(pCode);
		return NULL;
	}

	if (ppInputLayout)
	{
		hr = pDevice->CreateInputLayout(pInputElementDescs, numElements,
			pCode->GetBufferPointer(), pCode->GetBufferSize(), ppInputLayout);
		if (FAILED(hr)) {
			SafeRelease(pCode);
			return NULL;
		}
	}

	SafeRelease(pCode);

	return p;
}

PixelShader::PixelShader()
	: m_pPShader(NULL)
{ }

PixelShader::~PixelShader()
{
	SafeRelease(m_pPShader);
}

PixelShaderPtr PixelShader::Create(ID3D11Device* pDevice, const char* src, const char* entryPoint, const char* target)
{
	PixelShaderPtr p(new PixelShader);

	HRESULT hr;

	ID3DBlob* pCode = CompileShader(src, entryPoint, target);
	if (!pCode) {
		return NULL;
	}

	hr = pDevice->CreatePixelShader(pCode->GetBufferPointer(),
		pCode->GetBufferSize(), NULL, &p->m_pPShader);
	if (FAILED(hr)) {
		SafeRelease(pCode);
		return NULL;
	}

	SafeRelease(pCode);

	return p;
}

}
