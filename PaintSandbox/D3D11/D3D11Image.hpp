// D3D11Image.hpp
// Nolan Check
// Created 12/27/2011

#ifndef _D3D11IMAGE_HPP
#define _D3D11IMAGE_HPP

#include <D3D11.h>

#include <memory>

#include "Driver.hpp"
#include "Texture2D.hpp"

namespace D3D11
{

class D3D11Image;
typedef std::shared_ptr<D3D11Image> D3D11ImagePtr;

class D3D11Image : public DriverImage
{

public:

	static D3D11ImagePtr Create(ID3D11Device* pDevice, DXGI_FORMAT format, UINT width, UINT height);
	~D3D11Image();

	Texture2DPtr GetTex() { return m_texture; }
	ID3D11ShaderResourceView* GetSRV() { return m_pTextureSRV; }
	ID3D11RenderTargetView* GetRTV() { return m_pTextureRTV; }

private:

	D3D11Image();

	Texture2DPtr m_texture;
	ID3D11ShaderResourceView* m_pTextureSRV;
	ID3D11RenderTargetView* m_pTextureRTV;

};

}

#endif
