// D3D11Utils.hpp
// Nolan Check
// Created 12/26/2011

#ifndef _D3D11UTILS_HPP
#define _D3D11UTILS_HPP

#include <D3D11.h>

#include <memory>

#include "WindowsUtils.hpp"

namespace D3D11
{

IDXGIAdapter* GetDXGIAdapterFromD3D11Device(ID3D11Device* pDevice);
IDXGISurface* OpenD3D11TextureOnD3D10Device(ID3D11Texture2D* texture, ID3D10Device1* device);

ComPtr<ID3D11VertexShader> CreateVertexShaderFromCode(ID3D11Device* pDevice,
	const char* src, const char* entryPoint, const char* target,
	const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs = NULL, UINT numElements = 0,
	ComPtr<ID3D11InputLayout>* ppInputLayout = NULL);

ComPtr<ID3D11PixelShader> CreatePixelShaderFromCode(ID3D11Device* pDevice,
	const char* src, const char* entryPoint, const char* target);

}

#endif
