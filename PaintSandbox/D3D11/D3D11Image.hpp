// D3D11Image.hpp
// Nolan Check
// Created 12/27/2011

#ifndef _D3D11IMAGE_HPP
#define _D3D11IMAGE_HPP

#include <D3D11.h>

#include <memory>

#include "Driver.hpp"
#include "WindowsUtils.hpp"

namespace D3D11
{

class D3D11Image;
typedef std::shared_ptr<D3D11Image> D3D11ImagePtr;

class D3D11Image : public DriverImage
{

public:

	static D3D11ImagePtr Create(ID3D11Device* pDevice, DXGI_FORMAT format, UINT width, UINT height);

	ComPtr<ID3D11Texture2D> GetTex() { return m_texture; }
	ComPtr<ID3D11ShaderResourceView> GetSRV() { return m_srv; }
	ComPtr<ID3D11RenderTargetView> GetRTV() { return m_rtv; }

private:

	D3D11Image();

	ComPtr<ID3D11Texture2D> m_texture;
	ComPtr<ID3D11ShaderResourceView> m_srv;
	ComPtr<ID3D11RenderTargetView> m_rtv;

};

}

#endif
