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

VertexShaderPtr VertexShader::Create(ID3D11Device* pDevice, const char* src, const char* entryPoint, const char* target)
{
	VertexShaderPtr result(new VertexShader);
	if (!result->CreateInternal(pDevice, src, entryPoint, target)) {
		return NULL;
	}
	return result;
}

VertexShader::VertexShader()
	: m_pVShader(NULL)
{ }

VertexShader::~VertexShader()
{
	SafeRelease(m_pVShader);
}

bool VertexShader::CreateInternal(ID3D11Device* pDevice, const char* src, const char* entryPoint, const char* target)
{
	HRESULT hr;

	ID3DBlob* pCode = CompileShader(src, entryPoint, target);
	if (!pCode) {
		return false;
	}

	hr = pDevice->CreateVertexShader(pCode->GetBufferPointer(),
		pCode->GetBufferSize(), NULL, &m_pVShader);
	if (FAILED(hr)) {
		SafeRelease(pCode);
		return false;
	}

	SafeRelease(pCode);

	return true;
}

PixelShaderPtr PixelShader::Create(ID3D11Device* pDevice, const char* src, const char* entryPoint, const char* target)
{
	PixelShaderPtr result(new PixelShader);
	if (!result->CreateInternal(pDevice, src, entryPoint, target)) {
		return NULL;
	}
	return result;
}

PixelShader::PixelShader()
	: m_pPShader(NULL)
{ }

PixelShader::~PixelShader()
{
	SafeRelease(m_pPShader);
}

bool PixelShader::CreateInternal(ID3D11Device* pDevice, const char* src, const char* entryPoint, const char* target)
{
	HRESULT hr;

	ID3DBlob* pCode = CompileShader(src, entryPoint, target);
	if (!pCode) {
		return false;
	}

	hr = pDevice->CreatePixelShader(pCode->GetBufferPointer(),
		pCode->GetBufferSize(), NULL, &m_pPShader);
	if (FAILED(hr)) {
		SafeRelease(pCode);
		return false;
	}

	SafeRelease(pCode);

	return true;
}

}
