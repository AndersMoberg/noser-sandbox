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

}
