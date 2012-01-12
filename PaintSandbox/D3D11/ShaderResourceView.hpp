// ShaderResourceView.hpp
// Nolan Check
// Created 1/11/2012

#ifndef _SHADERRESOURCEVIEW_HPP
#define _SHADERRESOURCEVIEW_HPP

#include <memory>

#include "Texture2D.hpp"

namespace D3D11
{

class ShaderResourceView;
typedef std::shared_ptr<ShaderResourceView> ShaderResourceViewPtr;

class ShaderResourceView
{

public:

	~ShaderResourceView();
	static ShaderResourceViewPtr Create(ID3D11Device* pDevice, Texture2DPtr texture);

	ID3D11ShaderResourceView* Get() { return m_pSRV; }

private:

	ShaderResourceView();

	ID3D11ShaderResourceView* m_pSRV;

};

}

#endif
