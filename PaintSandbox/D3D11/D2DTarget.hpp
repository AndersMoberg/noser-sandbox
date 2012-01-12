// D2DTarget.hpp
// Nolan Check
// Created 1/10/2012

#ifndef _D2DTARGET_HPP
#define _D2DTARGET_HPP

#include <D3D11.h>
#include <D2D1.h>

#include <memory>

#include "Texture2D.hpp"

namespace D3D11
{

class D2DTarget;
typedef std::shared_ptr<D2DTarget> D2DTargetPtr;

class D2DTarget
{

public:

	~D2DTarget();
	static D2DTargetPtr Create(ID2D1Factory* pD2DFactory,
		ID3D11Device* pD3D11Device, ID3D10Device1* pD3D10Device,
		UINT width, UINT height);

	ID3D11ShaderResourceView* AcquireSRV();
	void ReleaseSRV();
	ID2D1RenderTarget* AcquireTarget();
	void ReleaseTarget();

private:

	D2DTarget();

	Texture2DPtr m_d2dTexture;
	ID3D11ShaderResourceView* m_pD2DTextureSRV;

	IDXGIKeyedMutex* m_pD3D11Mutex;
	IDXGIKeyedMutex* m_pD2DMutex;

	ID2D1RenderTarget* m_pD2DTarget;

};

}

#endif
