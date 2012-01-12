// Texture2D.hpp
// Nolan Check
// Created 1/11/2012

#ifndef _TEXTURE2D_HPP
#define _TEXTURE2D_HPP

#include <D3D11.h>

#include <memory>

namespace D3D11
{

class Texture2D;
typedef std::shared_ptr<Texture2D> Texture2DPtr;

class Texture2D
{

public:

	~Texture2D();
	static Texture2DPtr Create(ID3D11Device* pDevice, const D3D11_TEXTURE2D_DESC& t2dd);

	ID3D11Texture2D* Get() { return m_pTexture; }

private:

	Texture2D();

	ID3D11Texture2D* m_pTexture;

};

}

#endif
