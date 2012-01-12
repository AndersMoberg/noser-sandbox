// D3D11Image.hpp
// Nolan Check
// Created 12/27/2011

#ifndef _D3D11IMAGE_HPP
#define _D3D11IMAGE_HPP

#include <D3D11.h>

#include <memory>

#include "Driver.hpp"
#include "Texture2D.hpp"
#include "ShaderResourceView.hpp"
#include "RenderTargetView.hpp"

namespace D3D11
{

class D3D11Image;
typedef std::shared_ptr<D3D11Image> D3D11ImagePtr;

class D3D11Image : public DriverImage
{

public:

	static D3D11ImagePtr Create(ID3D11Device* pDevice, DXGI_FORMAT format, UINT width, UINT height);

	Texture2DPtr GetTex() { return m_texture; }
	ShaderResourceViewPtr GetSRV() { return m_srv; }
	RenderTargetViewPtr GetRTV() { return m_rtv; }

private:

	D3D11Image();

	Texture2DPtr m_texture;
	ShaderResourceViewPtr m_srv;
	RenderTargetViewPtr m_rtv;

};

}

#endif
