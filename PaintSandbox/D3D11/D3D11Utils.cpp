// D3D11Utils.cpp
// Nolan Check
// Created 12/30/2011

#include "D3D11Utils.hpp"

#include <D3Dcompiler.h>

#include "WindowsUtils.hpp"

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
		throw std::exception("Failed to compile HLSL shader");
	}

	return pCode;
}

IDXGIAdapter* GetDXGIAdapterFromD3D11Device(ID3D11Device* device)
{
	IDXGIDevice* dxgiDevice = NULL;
	CHECK_HR(device->QueryInterface(&dxgiDevice));

	IDXGIAdapter* dxgiAdapter = NULL;
	CHECK_HR(dxgiDevice->GetAdapter(&dxgiAdapter));

	SafeRelease(dxgiDevice);

	return dxgiAdapter;
}

IDXGISurface* OpenD3D11TextureOnD3D10Device(ID3D11Texture2D* texture, ID3D10Device1* device)
{
	// Obtain share handle for texture

	IDXGIResource* dxgiResource = NULL;
	CHECK_HR(texture->QueryInterface(IID_PPV_ARGS(&dxgiResource)));

	HANDLE shareHandle = NULL;
	CHECK_HR(dxgiResource->GetSharedHandle(&shareHandle));

	SafeRelease(dxgiResource);

	// Open D3D11 texture on D3D10 device

	IDXGISurface* dxgiSurface = NULL;
	CHECK_HR(device->OpenSharedResource(shareHandle, IID_PPV_ARGS(&dxgiSurface)));

	return dxgiSurface;
}

ComPtr<ID3D11VertexShader> CreateVertexShaderFromCode(ID3D11Device* pDevice,
	const char* src, const char* entryPoint, const char* target,
	const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT numElements,
	ComPtr<ID3D11InputLayout>* ppInputLayout)
{
	ID3DBlob* pCode = CompileShader(src, entryPoint, target);

	ComPtr<ID3D11VertexShader> shader;
	CHECK_HR(pDevice->CreateVertexShader(pCode->GetBufferPointer(),
		pCode->GetBufferSize(), NULL, shader.Receive()));

	if (ppInputLayout)
	{
		CHECK_HR(pDevice->CreateInputLayout(pInputElementDescs,
			numElements, pCode->GetBufferPointer(), pCode->GetBufferSize(),
			ppInputLayout->Receive()));
	}

	SafeRelease(pCode);
	return shader;
}

ComPtr<ID3D11PixelShader> CreatePixelShaderFromCode(ID3D11Device* pDevice,
	const char* src, const char* entryPoint, const char* target)
{
	ID3DBlob* pCode = CompileShader(src, entryPoint, target);

	ComPtr<ID3D11PixelShader> shader;
	CHECK_HR(pDevice->CreatePixelShader(pCode->GetBufferPointer(),
		pCode->GetBufferSize(), NULL, shader.Receive()));

	SafeRelease(pCode);
	return shader;
}

}
